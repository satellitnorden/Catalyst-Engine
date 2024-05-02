//Header file.
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//Core.
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Task.h>

//File.
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/Shader/CatalystToneMapping.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/WorldTracingSystem.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

/*
*	Rendering reference user interface scene class definition.
*/
class RenderingReferenceUserInterfaceScene final : public UserInterfaceScene
{

public:

	/*
	*	Callback for when this user interface scene is activated.
	*/
	FORCE_INLINE void OnActivated() NOEXCEPT override
	{
		//Call parent function.
		UserInterfaceScene::OnActivated();

		//Add the image.
		{
			UserInterfaceMaterial material;
			material.SetColor(Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f));

			_Image = AddImageByNormalizedCoordinate(Vector2<float32>(0.0f, 0.0f),
													Vector2<float32>(1.0f, 1.0f),
													material);
		}

		//Add the progress bar.
		{
			_ProgressBar = AddProgressBarByNormalizedCoordinate(Vector2<float32>(0.0f, 0.95f),
																Vector2<float32>(1.0f, 1.0f),
																"Current Number Of Samples: 0");
		}
	}

	/*
	*	Callback for when this user interface scene is deactivated.
	*/
	FORCE_INLINE void OnDeactivated() NOEXCEPT override
	{
		//Call parent function.
		UserInterfaceScene::OnDeactivated();

		//Reset the progress bar.
		_ProgressBar = nullptr;
	}

	/*
	*	Sets the texture.
	*/
	FORCE_INLINE void SetTextureIndex(const uint32 value) NOEXCEPT
	{
		UserInterfaceMaterial material;
		material.SetTextureIndex(value);

		_Image->SetMaterial(material);
	}

	/*
	*	Sets the current number of samples.
	*/
	FORCE_INLINE void SetCurrentNumberOfSamples(const uint64 value) NOEXCEPT
	{
		char buffer[128];
		sprintf_s(buffer, "Current Number Of Samples: %llu", value);

		_ProgressBar->SetText(buffer);
	}

	/*
	*	Sets the progress.
	*/
	FORCE_INLINE void SetProgress(const float32 value) NOEXCEPT
	{
		if (_ProgressBar)
		{
			_ProgressBar->SetCurrentProgress(value);
		}
	}

private:

	//The image.
	UserInterfaceImage *RESTRICT _Image;

	//The progress bar.
	UserInterfaceProgressBar *RESTRICT _ProgressBar{ nullptr };

};

//Rendering reference system data.
namespace RenderingReferenceSystemData
{
	//Denotes whether or not rendering reference is in progress.
	bool _RenderingReferenceIsInProgress{ false };

	//The update identifier.
	uint64 _UpdateIdentifier;

	//The intermediate texture.
	Texture2D<Vector4<float64>> _IntermediateTexture;

	//The current number of samples.
	uint64 _CurrentNumberOfSamples;

	//The tasks.
	DynamicArray<Task> _Tasks;

	//The queued task data.
	AtomicQueue<uint32, 4'096, AtomicQueueMode::SINGLE, AtomicQueueMode::MULTIPLE> _QueuedTaskData;

	//The final texture.
	Texture2D<Vector4<float32>> _FinalTexture;

	//The final texture handle.
	Texture2DHandle _FinalTextureHandle{ EMPTY_HANDLE };

	//The final texture index.
	uint32 _FinalTextureIndex{ 0 };

	//The rendering reference user interface scene.
	RenderingReferenceUserInterfaceScene _RenderingReferenceUserInterfaceScene;

	//The current number of pixels.
	Atomic<uint64> _CurrentNumberOfPixels{ 0 };

	//The total number of pixels.
	uint64 _TotalNumberOfPixels;
}

//Rendering reference system logic.
class RenderingReferenceSystemLogic
{

public:

	/*
	*	Starts the rendering reference process.
	*/
	FORCE_INLINE static void Start() NOEXCEPT
	{
		//Pause the Catalyst engine system.
		CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);

		//Tell the world tracing system to cache the world state.
		WorldTracingSystem::Instance->CacheWorldState();

		//Reset the intermediate texture.
		if (RenderingReferenceSystemData::_IntermediateTexture.GetWidth() == 0
			|| RenderingReferenceSystemData::_IntermediateTexture.GetHeight() == 0)
		{
			Resolution default_resolution;
			CatalystPlatform::GetDefaultResolution(&default_resolution._Width, &default_resolution._Height);

			RenderingReferenceSystemData::_IntermediateTexture.Initialize(default_resolution._Width, default_resolution._Height);
		}

		Memory::Set(RenderingReferenceSystemData::_IntermediateTexture.Data(), 0, RenderingReferenceSystemData::_IntermediateTexture.GetWidth() * RenderingReferenceSystemData::_IntermediateTexture.GetHeight() * sizeof(Vector4<float64>));

		//Reset the current number of samples.
		RenderingReferenceSystemData::_CurrentNumberOfSamples = 1;

		//Reset the final texture.
		if (RenderingReferenceSystemData::_FinalTexture.GetWidth() == 0
			|| RenderingReferenceSystemData::_FinalTexture.GetHeight() == 0)
		{
			Resolution default_resolution;
			CatalystPlatform::GetDefaultResolution(&default_resolution._Width, &default_resolution._Height);

			RenderingReferenceSystemData::_FinalTexture.Initialize(default_resolution._Width, default_resolution._Height);
		}

		//Initialize the tasks.
		{
			const uint32 number_of_task_executors{ TaskSystem::Instance->GetNumberOfTaskExecutors() - 4 };

			RenderingReferenceSystemData::_Tasks.Upsize<false>(number_of_task_executors);

			uint32 texel_rows_per_task{ RenderingReferenceSystemData::_IntermediateTexture.GetHeight() / number_of_task_executors };

			uint32 current_y{ 0 };

			for (uint64 i{ 0 }; i < number_of_task_executors; ++i)
			{
				RenderingReferenceSystemData::_Tasks[i]._Function = [](void *const RESTRICT)
				{
					TaskUpdate();
				};
				RenderingReferenceSystemData::_Tasks[i]._Arguments = nullptr;
				RenderingReferenceSystemData::_Tasks[i]._ExecutableOnSameThread = false;

				current_y += texel_rows_per_task;
			}
		}

		//Activate the rendering reference user interface scene.
		UserInterfaceSystem::Instance->ActivateScene(&RenderingReferenceSystemData::_RenderingReferenceUserInterfaceScene);

		//Set the total number of pixels.
		RenderingReferenceSystemData::_TotalNumberOfPixels = RenderingReferenceSystemData::_FinalTexture.GetWidth() * RenderingReferenceSystemData::_FinalTexture.GetHeight();

		//Fire the tasks.
		FireTasks();

		//Register the update.
		RenderingReferenceSystemData::_UpdateIdentifier = CatalystEngineSystem::Instance->RegisterUpdate(	[](void* const RESTRICT)
																											{
																												RenderingReferenceSystemLogic::PreUpdate();
																											},
																											nullptr,
																											UpdatePhase::PRE,
																											UpdatePhase::USER_INTERFACE,
																											false,
																											false);

		//Rendering reference is now in progress!
		RenderingReferenceSystemData::_RenderingReferenceIsInProgress = true;
	}

	/*
	*	Updates the rendering reference system during the PRE update phase.
	*/
	FORCE_INLINE static void PreUpdate() NOEXCEPT
	{
		//Update the progress.
		RenderingReferenceSystemData::_RenderingReferenceUserInterfaceScene.SetProgress(static_cast<float32>(RenderingReferenceSystemData::_CurrentNumberOfPixels) / static_cast<float32>(RenderingReferenceSystemData::_TotalNumberOfPixels));

		//Are all tasks done?
		if (AllTasksDone())
		{
			//Create a new final texture handle.
			Texture2DHandle new_final_texture_handle;
			RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(RenderingReferenceSystemData::_FinalTexture), TextureFormat::RGBA_FLOAT32, TextureUsage::NONE, false), &new_final_texture_handle);

			//Add the new final texture to the global render data.
			const uint32 new_final_texture_index{ RenderingSystem::Instance->AddTextureToGlobalRenderData(new_final_texture_handle) };

			//Tell the user interface scene the new texture index.
			RenderingReferenceSystemData::_RenderingReferenceUserInterfaceScene.SetTextureIndex(new_final_texture_index);

			//Tell the user interface scene the current number of samples.
			RenderingReferenceSystemData::_RenderingReferenceUserInterfaceScene.SetCurrentNumberOfSamples(RenderingReferenceSystemData::_CurrentNumberOfSamples);

			//Remove the old final texture, if there is one.
			if (RenderingReferenceSystemData::_FinalTextureHandle)
			{
				RenderingSystem::Instance->ReturnTextureToGlobalRenderData(RenderingReferenceSystemData::_FinalTextureIndex);
				RenderingSystem::Instance->DestroyTexture2D(&RenderingReferenceSystemData::_FinalTextureHandle);
			}

			//Update the final texture handle/index.
			RenderingReferenceSystemData::_FinalTextureHandle = new_final_texture_handle;
			RenderingReferenceSystemData::_FinalTextureIndex = new_final_texture_index;

			//Increment the current number of samples.
			++RenderingReferenceSystemData::_CurrentNumberOfSamples;

			//Fire the tasks again!
			FireTasks();
		}

		else
		{
			Concurrency::CurrentThread::Yield();
		}
	}

	/*
	*	Fires off the tasks.
	*/
	FORCE_INLINE static void FireTasks() NOEXCEPT
	{
		LOG_INFORMATION("Performing iteration %llu", RenderingReferenceSystemData::_CurrentNumberOfSamples);

		//Reset the current number of pixels.
		RenderingReferenceSystemData::_CurrentNumberOfPixels = 0;

		//Fill the queued task data, in an interleaved manner so that there are less false sharing.
		for (uint32 Y{ 0 }; Y < RenderingReferenceSystemData::_IntermediateTexture.GetHeight(); Y += 2)
		{
			RenderingReferenceSystemData::_QueuedTaskData.Push(Y);
		}

		for (uint32 Y{ 1 }; Y < RenderingReferenceSystemData::_IntermediateTexture.GetHeight(); Y += 2)
		{
			RenderingReferenceSystemData::_QueuedTaskData.Push(Y);
		}

		for (Task &task : RenderingReferenceSystemData::_Tasks)
		{
			TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &task);
		}
	}

	/*
	*	Returns whether or not all tasks are done.
	*/
	FORCE_INLINE NO_DISCARD static bool AllTasksDone() NOEXCEPT
	{
		bool all_tasks_done{ true };

		for (const Task &task : RenderingReferenceSystemData::_Tasks)
		{
			all_tasks_done &= task.IsExecuted();
		}

		return all_tasks_done;
	}

	/*
	*	Update for the task.
	*/
	FORCE_INLINE static void TaskUpdate() NOEXCEPT
	{
		while (const uint32 *const RESTRICT Y{ RenderingReferenceSystemData::_QueuedTaskData.Pop() })
		{
			for (uint32 X{ 0 }; X < RenderingReferenceSystemData::_IntermediateTexture.GetWidth(); ++X)
			{
				//Calculate the jitter.
				const Vector2<float32> jitter{ CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f), CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f) };

				//Calculate the normalized coordinate.
				const Vector2<float32> normalized_coordinate{	(static_cast<float32>(X) + 0.5f + jitter._X) / static_cast<float32>(RenderingReferenceSystemData::_IntermediateTexture.GetWidth()),
																1.0f - ((static_cast<float32>(*Y) + 0.5f + jitter._Y) / static_cast<float32>(RenderingReferenceSystemData::_IntermediateTexture.GetHeight())) };

				//Calculate the ray.
				Ray ray;

				ray.SetOrigin(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition());
				ray.SetDirection(RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(normalized_coordinate));

				//Retrieve the radiance.
				const Vector3<float32> radiance{ WorldTracingSystem::Instance->RadianceRay(ray) };

				//Write to the intermediate texture.
				Vector4<float64>& intermediate_sample{ RenderingReferenceSystemData::_IntermediateTexture.At(X, *Y) };
				intermediate_sample += Vector4<float64>(static_cast<float64>(radiance._R), static_cast<float64>(radiance._G), static_cast<float64>(radiance._B), 1.0);

				//Calculate the final radiance.
				Vector3<float32> final_radiance;

				final_radiance._R = static_cast<float32>(intermediate_sample._R / static_cast<float64>(RenderingReferenceSystemData::_CurrentNumberOfSamples));
				final_radiance._G = static_cast<float32>(intermediate_sample._G / static_cast<float64>(RenderingReferenceSystemData::_CurrentNumberOfSamples));
				final_radiance._B = static_cast<float32>(intermediate_sample._B / static_cast<float64>(RenderingReferenceSystemData::_CurrentNumberOfSamples));

				//Apply tone mapping.
				final_radiance = CatalystToneMapping::ApplyToneMapping(final_radiance);

				//Write to the final texture.
				Vector4<float32>& final_sample{ RenderingReferenceSystemData::_FinalTexture.At(X, *Y) };

				final_sample._R = BaseMath::Clamp<float32>(final_radiance._R, 0.0f, 1.0f);
				final_sample._G = BaseMath::Clamp<float32>(final_radiance._G, 0.0f, 1.0f);
				final_sample._B = BaseMath::Clamp<float32>(final_radiance._B, 0.0f, 1.0f);
				final_sample._A = 1.0f;

				//Update the current number of pixels.
				++RenderingReferenceSystemData::_CurrentNumberOfPixels;
			}
		}
	}

	/*
	*	Stops the rendering reference process.
	*/
	FORCE_INLINE static void Stop(const char *const RESTRICT file_path) NOEXCEPT
	{
		//De-register the update.
		CatalystEngineSystem::Instance->DeregisterUpdate(RenderingReferenceSystemData::_UpdateIdentifier);

		//Wait for all the tasks to finish.
		while (!AllTasksDone());

		//Write the rendered image to file, if a file path is specified.
		if (file_path)
		{
			//Flip the final texture before writing it.
			Texture2D<Vector4<float32>> flipped_finale_texture{ RenderingReferenceSystemData::_FinalTexture.GetWidth(), RenderingReferenceSystemData::_FinalTexture.GetHeight() };

			for (uint32 Y{ 0 }; Y < RenderingReferenceSystemData::_FinalTexture.GetHeight(); ++Y)
			{
				for (uint32 X{ 0 }; X < RenderingReferenceSystemData::_FinalTexture.GetWidth(); ++X)
				{
					flipped_finale_texture.At(X, Y) = RenderingReferenceSystemData::_FinalTexture.At(X, RenderingReferenceSystemData::_FinalTexture.GetHeight() - 1 - Y);
				}
			}

			PNGWriter::Write(flipped_finale_texture, file_path);
		}

		//Resume the Catalyst engine system.
		CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);

		//Deactivate the rendering reference user interface scene.
		UserInterfaceSystem::Instance->DeactivateScene(&RenderingReferenceSystemData::_RenderingReferenceUserInterfaceScene);

		//Rendering reference is no longer in progress!
		RenderingReferenceSystemData::_RenderingReferenceIsInProgress = false;
	}

};

/*
*	Returns whether or not rendering reference is in progress.
*/
NO_DISCARD bool RenderingReferenceSystem::IsRenderingReferenceInProgress() const NOEXCEPT
{
	return RenderingReferenceSystemData::_RenderingReferenceIsInProgress;
}

/*
*	Starts the rendering reference.
*/
void RenderingReferenceSystem::StartRenderingReference() NOEXCEPT
{
	RenderingReferenceSystemLogic::Start();
}

/*
*	Stops the rendering reference. Takes an optional file path to output the image.
*/
void RenderingReferenceSystem::StopRenderingReference(const char *const RESTRICT file_path) NOEXCEPT
{
	RenderingReferenceSystemLogic::Stop(file_path);
}