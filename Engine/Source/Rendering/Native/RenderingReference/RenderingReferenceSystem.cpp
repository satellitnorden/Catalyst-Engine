#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
//Header file.
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/RenderPasses/RenderingReferenceRenderPass.h>

//Systems.
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>

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
			_RenderingReferenceInProgress = false;
		}

		//Otherwise, start it.
		else
		{
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

			//Reset the iterations.
			_Iterations = 0;
		}
	}

	//If rendering reference is in progress, do one iteration on the texture.
	if (_RenderingReferenceInProgress)
	{
		UpdateRenderingReference();
	}
}

/*
*	Updates the rendering reference.
*/
void RenderingReferenceSystem::UpdateRenderingReference() NOEXCEPT
{
	//Calculate all texels.
	for (uint32 Y{ 0 }; Y < RenderingSystem::Instance->GetScaledResolution()._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			CalculateTexel(X, Y);
		}
	}

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
										1.0f - ((static_cast<float>(Y) + 0.5f) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height)) };

	//Calculate the ray direction.
	return RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(coordinate);
}
#endif