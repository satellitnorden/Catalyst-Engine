#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
//Header file.
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//File.
#include <File/Writers/TGAWriter.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/RenderPasses/RenderingReferenceRenderPass.h>

//Systems.
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Rendering reference system constants.
namespace RenderingReferenceSystemConstants
{
	constexpr uint32 SLIZE_SIZE{ 32 };
}

/*
*	Updates the rendering reference system during the render update phase.
*/
void RenderingReferenceSystem::RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Should the rendering reference start/stop?
	const KeyboardState *const RESTRICT state{ InputSystem::Instance->GetKeyboardState() };

	if (state->GetButtonState(KeyboardButton::NumpadFive) == ButtonState::Pressed)
	{
		//Is the rendering reference in progress? If so, stop it.
		if (_RenderingReferenceInProgress)
		{
			EndRenderingReference();
		}

		//Otherwise, start it.
		else
		{
			StartRenderingReference();
		}
	}

	//If rendering reference is in progress, do one iteration on the texture.
	if (_RenderingReferenceInProgress)
	{
		UpdateRenderingReference();
	}
}

/*
*	Starts the rendering reference.
*/
void RenderingReferenceSystem::StartRenderingReference() NOEXCEPT
{
	//Start the rendering reference progress.
	_RenderingReferenceInProgress = true;

	//Initialize the texture.
	_RenderingReferenceTexture.Initialize(RenderingSystem::Instance->GetScaledResolution()._Width, RenderingSystem::Instance->GetScaledResolution()._Height);

	for (uint32 Y{ 0 }; Y < RenderingSystem::Instance->GetScaledResolution()._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			_RenderingReferenceTexture.At(X, Y) = Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	//Calculate all the synchronous data.
	uint32 current_slice{ 0 };
	const uint32 maximum_y{ RenderingSystem::Instance->GetScaledResolution()._Height };

	for (;;)
	{
		//Add new asynchronous data.
		_AsynchronousData.EmplaceSlow();

		AsynchronousData& data{ _AsynchronousData.Back() };

		data._Task._Function = [](void *const RESTRICT arguments)
		{
			RenderingSystem::Instance->GetRenderingReferenceSystem()->ExecuteAsynchronous(static_cast<const AsynchronousData* const RESTRICT>(arguments));
		};
		data._Task._Arguments = &data;
		data._Task._ExecutableOnSameThread = false;
		data._StartY = current_slice;
		data._EndY = CatalystBaseMath::Minimum<uint32>(current_slice + RenderingReferenceSystemConstants::SLIZE_SIZE, maximum_y);

		current_slice += RenderingReferenceSystemConstants::SLIZE_SIZE;

		if (current_slice >= maximum_y)
		{
			break;
		}
	}

	//Reset the iterations.
	_Iterations = 0;
}

/*
*	Ends the rendering reference.
*/
void RenderingReferenceSystem::EndRenderingReference() NOEXCEPT
{
	//End the rendering reference progress.
	_RenderingReferenceInProgress = false;

	//Normalize the values in the rendering reference texture.
	for (uint32 Y{ 0 }; Y < RenderingSystem::Instance->GetScaledResolution()._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			_RenderingReferenceTexture.At(X, Y) /= static_cast<float>(_Iterations);
		}
	}

	//Write the image to file.
	TGAWriter::Write(_RenderingReferenceTexture, "RenderingReference.tga");
}

/*
*	Updates the rendering reference.
*/
void RenderingReferenceSystem::UpdateRenderingReference() NOEXCEPT
{
	//Execute all tasks.
	for (AsynchronousData &data : _AsynchronousData)
	{
		TaskSystem::Instance->ExecuteTask(&data._Task);
	}

	//Wait for all tasks.
	for (const AsynchronousData& data : _AsynchronousData)
	{
		data._Task.WaitFor();
	}

	//Update the number of iterations.
	++_Iterations;

	//Recreate the texture.
	if (_RenderingReferenceTextureHandle)
	{
		RenderingSystem::Instance->ReturnTextureToGlobalRenderData(_RenderingReferenceTextureIndex);
		RenderingSystem::Instance->DestroyTexture2D(&_RenderingReferenceTextureHandle);
	}

	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(_RenderingReferenceTexture), TextureFormat::R32G32B32A32_Float), &_RenderingReferenceTextureHandle);
	_RenderingReferenceTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(_RenderingReferenceTextureHandle);

	//Set the properties for the rendering reference.
	RenderingReferenceRenderPass::Instance->SetProperties(_RenderingReferenceTextureIndex, _Iterations);
}

/*
*	Executes asynchronously.
*/
void RenderingReferenceSystem::ExecuteAsynchronous(const AsynchronousData *const RESTRICT data) NOEXCEPT
{
	//Calculate all texels.
	for (uint32 Y{ data->_StartY }; Y < data->_EndY; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			CalculateTexel(X, Y);
		}
	}
}

/*
*	Calculates a texel.
*/
void RenderingReferenceSystem::CalculateTexel(const uint32 X, const uint32 Y) NOEXCEPT
{
	//Calculate the ray direction.
	const Vector3<float> ray_direction{ CalculateRayDirection(X, Y) };


	//Calculate the upward factor.
	const float upward_factor{ Vector3<float>::DotProduct(ray_direction, VectorConstants::UP) };

	_RenderingReferenceTexture.At(X, Y) += CatalystRandomMath::RandomChance(upward_factor) ? Vector4<float>(0.0f, 1.0f, 1.0f, 1.0f) : Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f);
}

/*
*	Calculates the ray direction.
*/
Vector3<float> RenderingReferenceSystem::CalculateRayDirection(const uint32 X, const uint32 Y) NOEXCEPT
{
	//Calculate the coordinate.
	const Vector2<float> coordinate{	(static_cast<float>(X) + 0.5f) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width),
										(static_cast<float>(Y) + 0.5f) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height) };

	//Calculate the ray direction.
	return RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(coordinate);
}
#endif