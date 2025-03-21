//Header file.
#include <Systems/PathTracingSystem.h>

//Components.
#include <Components/Core/Component.h>

//File.
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Path tracing.
#include <PathTracing/PathTracingCore.h>

//Rendering.
#include <Rendering/Native/Shader/CatalystToneMapping.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//STL.
#include <stdio.h>

//Constants.
#define NEW_GATHER_TRIANGLES (0)

#if !NEW_GATHER_TRIANGLES
#include <Systems/WorldTracingSystem.h>
#endif

/*
*	Path tracing user interface scene class definition.
*/
class PathTracingUserInterfaceScene final : public UserInterfaceScene
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

			_Image = AddImageByNormalizedCoordinate
			(
				Vector2<float32>(0.0f, 0.0f),
				Vector2<float32>(1.0f, 1.0f),
				material
			);
		}

		//Add the progress bar.
		_ProgressBar = AddProgressBarByNormalizedCoordinate
		(
			Vector2<float32>(0.0f, 0.0f),
			Vector2<float32>(1.0f, 0.025f)
		);

		//Add the text.
		_Text = AddTextByNormalizedCoordinate
		(
			Vector2<float32>(0.0f, 0.025f),
			Vector2<float32>(1.0f, 0.075f),
			"Current Number Of Samples: 0"
		);
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

		//Reset the text.
		_Text = nullptr;
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
	*	Sets the progress.
	*/
	FORCE_INLINE void SetProgress(const float32 value) NOEXCEPT
	{
		if (_ProgressBar)
		{
			_ProgressBar->SetCurrentProgress(value);
		}
	}

	/*
	*	Sets the samples.
	*/
	FORCE_INLINE void SetSamples(const uint64 value) NOEXCEPT
	{
		if (_Text)
		{
			char buffer[128];
			sprintf_s(buffer, "Samples: %llu", value);

			_Text->SetText(buffer);
		}
	}

private:

	//The image.
	UserInterfaceImage *RESTRICT _Image;

	//The progress bar.
	UserInterfaceProgressBar *RESTRICT _ProgressBar{ nullptr };

	//The text.
	UserInterfaceText *RESTRICT _Text{ nullptr };

};

//The user interface scene.
PathTracingUserInterfaceScene USER_INTERFACE_SCENE;

/*
*	Starts path tracing.
*/
void PathTracingSystem::Start() NOEXCEPT
{
	//Pause the Catalyst engine system.
	CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);

	//Tell the world tracing system to cache the world state.
	WorldTracingSystem::Instance->CacheWorldState();

#if NEW_GATHER_TRIANGLES
	//Gather the path tracing triangles.
	DynamicArray<Vertex> vertices;
	DynamicArray<PathTracingTriangle> triangles;

	Components::GatherPathTracingTriangles(&vertices, &triangles);
#else

#endif

	//Reset the intermediate texture.
	if (_IntermediateTexture.GetWidth() == 0
		|| _IntermediateTexture.GetHeight() == 0)
	{
		Resolution default_resolution;
		CatalystPlatform::GetDefaultResolution(&default_resolution._Width, &default_resolution._Height);

		_IntermediateTexture.Initialize(default_resolution._Width, default_resolution._Height);
	}

	Memory::Set(_IntermediateTexture.Data(), 0, _IntermediateTexture.GetWidth() * _IntermediateTexture.GetHeight() * sizeof(Vector4<float64>));

	//Reset the final texture.
	if (_FinalTexture.GetWidth() == 0
		|| _FinalTexture.GetHeight() == 0)
	{
		Resolution default_resolution;
		CatalystPlatform::GetDefaultResolution(&default_resolution._Width, &default_resolution._Height);

		_FinalTexture.Initialize(default_resolution._Width, default_resolution._Height);
	}

	//Reset the current number of samples.
	_CurrentNumberOfSamples = 1;

	//Calculate the total number of pixels.
	_TotalNumberOfPixels = _FinalTexture.GetWidth() * _FinalTexture.GetHeight();

	//Set up the tasks.
	_Tasks.Upsize<false>(TaskSystem::Instance->GetNumberOfTaskExecutors() - 2);

	for (Task &task : _Tasks)
	{
		task._Function = [](void* const RESTRICT)
		{
			PathTracingSystem::Instance->UpdateTask();
		};
		task._Arguments = nullptr;
		task._ExecutableOnSameThread = false;
	}

	//Execute the tasks.
	ExecuteTasks();

	//Activate the user interface scene.
	UserInterfaceSystem::Instance->ActivateScene(&USER_INTERFACE_SCENE);

	//Register the update.
	_UpdateIdentifier = CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT)
		{
			PathTracingSystem::Instance->Update();
		},
		nullptr,
		UpdatePhase::PRE,
		UpdatePhase::USER_INTERFACE,
		false,
		false
	);

	//Path tracing is now in progress!
	_IsInProgress = true;
}

/*
*	Stops path tracing.
*/
void PathTracingSystem::Stop(const char* const RESTRICT file_path) NOEXCEPT
{
	//Wait for all the tasks to finish.
	while (!AllTasksDone())
	{
		TaskSystem::Instance->DoWork(Task::Priority::LOW);
	}

	//Write the rendered image to file, if a file path is specified.
	if (file_path)
	{
		//Flip the final texture before writing it.
		Texture2D<Vector4<float32>> flipped_finale_texture{ _FinalTexture.GetWidth(), _FinalTexture.GetHeight() };

		for (uint32 Y{ 0 }; Y < _FinalTexture.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < _FinalTexture.GetWidth(); ++X)
			{
				flipped_finale_texture.At(X, Y) = _FinalTexture.At(X, _FinalTexture.GetHeight() - 1 - Y);
			}
		}

		PNGWriter::Write(flipped_finale_texture, file_path);
	}

	//Deactivate the rendering reference user interface scene.
	UserInterfaceSystem::Instance->DeactivateScene(&USER_INTERFACE_SCENE);

	//De-register the update.
	CatalystEngineSystem::Instance->DeregisterUpdate(_UpdateIdentifier);

	//Resume the Catalyst engine system.
	CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);

	//Path tracing is no longer in progress!
	_IsInProgress = false;
}

/*
*	Returns if path tracing is in progress.
*/
NO_DISCARD bool PathTracingSystem::IsInProgress() const NOEXCEPT
{
	return _IsInProgress;
}

/*
*	Updates the path tracing system.
*/
void PathTracingSystem::Update() NOEXCEPT
{
	//Update the progress.
	USER_INTERFACE_SCENE.SetProgress(static_cast<float32>(_CurrentNumberOfPixels.Load()) / static_cast<float32>(_TotalNumberOfPixels));

	//Are all tasks done?
	if (AllTasksDone())
	{
		//Create a new final texture handle.
		Texture2DHandle new_final_texture_handle;
		RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(_FinalTexture), TextureFormat::RGBA_FLOAT32, TextureUsage::NONE, false), &new_final_texture_handle);

		//Add the new final texture to the global render data.
		const uint32 new_final_texture_index{ RenderingSystem::Instance->AddTextureToGlobalRenderData(new_final_texture_handle) };

		//Tell the user interface scene the new texture index.
		USER_INTERFACE_SCENE.SetTextureIndex(new_final_texture_index);

		//Tell the user interface scene the of samples.
		USER_INTERFACE_SCENE.SetSamples(_CurrentNumberOfSamples);

		//Remove the old final texture, if there is one.
		if (_FinalTextureHandle)
		{
			RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_FinalTextureIndex);
			RenderingSystem::Instance->DestroyTexture2D(&_FinalTextureHandle);
		}

		//Update the final texture handle/index.
		_FinalTextureHandle = new_final_texture_handle;
		_FinalTextureIndex = new_final_texture_index;

		//Increment the current number of samples.
		++_CurrentNumberOfSamples;

		//Execute the tasks again!
		ExecuteTasks();
	}
}

/*
*	Executes the tasks.
*/
void PathTracingSystem::ExecuteTasks() NOEXCEPT
{
	//Reset the current number of pixels.
	_CurrentNumberOfPixels.Store(0);

	//Fill the queued rows, in an interleaved manner so that there are less false sharing.
	for (uint32 Y{ 0 }; Y < _IntermediateTexture.GetHeight(); Y += 4)
	{
		_QueuedRows.Push(Y);
	}

	for (uint32 Y{ 2 }; Y < _IntermediateTexture.GetHeight(); Y += 4)
	{
		_QueuedRows.Push(Y);
	}

	for (uint32 Y{ 1 }; Y < _IntermediateTexture.GetHeight(); Y += 4)
	{
		_QueuedRows.Push(Y);
	}

	for (uint32 Y{ 3 }; Y < _IntermediateTexture.GetHeight(); Y += 4)
	{
		_QueuedRows.Push(Y);
	}

	//Execute all tasks!
	for (Task &task : _Tasks)
	{
		TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, &task);
	}
}

/*
*	Updates a task.
*/
void PathTracingSystem::UpdateTask()
{
	//Pop a queued row.
	Optional<uint32> queued_row{ _QueuedRows.Pop() };

	while (queued_row.Valid())
	{
		//Iterate over all pixels.
		for (uint32 X{ 0 }; X < _IntermediateTexture.GetWidth(); ++X)
		{
			//Calculate the jitter.
			const Vector2<float32> jitter{ CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f), CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f) };

			//Calculate the normalized coordinate.
			const Vector2<float32> normalized_coordinate{ (static_cast<float32>(X) + 0.5f + jitter._X) / static_cast<float32>(_IntermediateTexture.GetWidth()),
															1.0f - ((static_cast<float32>(queued_row.Get()) + 0.5f + jitter._Y) / static_cast<float32>(_IntermediateTexture.GetHeight())) };

			//Calculate the ray.
			Ray ray;

			ray.SetOrigin(RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetAbsolutePosition());
			ray.SetDirection(RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(normalized_coordinate));

			//Retrieve the radiance.
			Vector3<float32> radiance;

			{
				//Set up the parameters.
				WorldTracingSystem::RadianceRayParameters parameters;

				parameters._ImprovedRayGeneration = X > 960;

				radiance = WorldTracingSystem::Instance->RadianceRay(ray, &parameters);
			}

			//Write to the intermediate texture.
			Vector4<float64>& intermediate_sample{ _IntermediateTexture.At(X, queued_row.Get()) };
			intermediate_sample += Vector4<float64>(static_cast<float64>(radiance._R), static_cast<float64>(radiance._G), static_cast<float64>(radiance._B), 1.0);

			//Calculate the final radiance.
			Vector3<float32> final_radiance;

			final_radiance._R = static_cast<float32>(intermediate_sample._R / static_cast<float64>(_CurrentNumberOfSamples));
			final_radiance._G = static_cast<float32>(intermediate_sample._G / static_cast<float64>(_CurrentNumberOfSamples));
			final_radiance._B = static_cast<float32>(intermediate_sample._B / static_cast<float64>(_CurrentNumberOfSamples));

			//Apply tone mapping.
			final_radiance = CatalystToneMapping::ApplyToneMapping(final_radiance);

			//Write to the final texture.
			Vector4<float32>& final_sample{ _FinalTexture.At(X, queued_row.Get()) };

			final_sample._R = BaseMath::Clamp<float32>(final_radiance._R, 0.0f, 1.0f);
			final_sample._G = BaseMath::Clamp<float32>(final_radiance._G, 0.0f, 1.0f);
			final_sample._B = BaseMath::Clamp<float32>(final_radiance._B, 0.0f, 1.0f);
			final_sample._A = 1.0f;

			//Update the current number of pixels.
			_CurrentNumberOfPixels.FetchAdd(1);
		}

		queued_row = _QueuedRows.Pop();
	}
}

/*
*	Returns if all tasks are done.
*/
NO_DISCARD bool PathTracingSystem::AllTasksDone() const NOEXCEPT
{
	bool all_tasks_done{ true };

	for (const Task &task : _Tasks)
	{
		all_tasks_done &= task.IsExecuted();
	}

	return all_tasks_done;
}