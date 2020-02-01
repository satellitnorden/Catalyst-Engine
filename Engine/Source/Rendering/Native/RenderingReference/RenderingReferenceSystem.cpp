#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
//Header file.
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//File.
#include <File/Writers/TGAWriter.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/Ray.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/RenderPasses/RenderingReferenceRenderPass.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>

//Rendering reference system constants.
namespace RenderingReferenceSystemConstants
{
	constexpr uint32 SLIZE_SIZE{ 16 };
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

	//Set the update speed to zero.
	CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);
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

	//Set the update speed back to normal.
	CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);
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
	//Calculate the ray.
	Ray ray;

	ray._Origin = Perceiver::Instance->GetPosition();
	ray._Direction = CalculateRayDirection(X, Y);
	ray._MaximumHitDistance = FLOAT_MAXIMUM;

	//Cast a ray against the scene and determine the color.
	const Vector4<float> color{ Vector4<float>(CastRayScene(ray, 0), 1.0f) };

	//Perform some post processing...

	//Write the color.
	_RenderingReferenceTexture.At(X, Y) += color;
}

/*
*	Calculates the ray direction.
*/
Vector3<float> RenderingReferenceSystem::CalculateRayDirection(const uint32 X, const uint32 Y) NOEXCEPT
{
	//Define constants.
	constexpr float JITTER{ 0.5f };

	//Calculate the coordinate.
	const Vector2<float> coordinate{	(static_cast<float>(X) + 0.5f + CatalystRandomMath::RandomFloatInRange(-JITTER, JITTER)) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width),
										(static_cast<float>(Y) + 0.5f + CatalystRandomMath::RandomFloatInRange(-JITTER, JITTER)) / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Height) };

	//Calculate the ray direction.
	return RenderingUtilities::CalculateRayDirectionFromScreenCoordinate(coordinate);
}

/*
*	Casts a ray against the scene. Returns the color.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CastRayScene(const Ray& ray, const uint8 recursion) NOEXCEPT
{
	//Cast a rays against the scene.
	SurfaceDescription surface_description;
	float hit_distance{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._ViewDistance };
	bool has_hit{ false };

	has_hit |= CastRayVolumetricParticles(ray, &surface_description, &hit_distance);
	has_hit |= CastRayTerrain(ray, &surface_description, &hit_distance);

	//Determine the color.
	if (has_hit)
	{
		return CalculateLighting(surface_description);
	}

	else
	{
		return CastRaySky(ray);
	}
}

/*
*	Casts a ray against the volumetric particles.
*/
NO_DISCARD bool RenderingReferenceSystem::CastRayVolumetricParticles(const Ray& ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT
{
	//Determine if a volumetric particle was hit.
	if (CatalystRandomMath::RandomChance(0.5f))
	{
		//Fill in the surface description/hit distance.
		surface_description->_Albedo = Vector3<float>(0.5f, 0.75f, 1.0f);
		surface_description->_Normal = VectorConstants::UP;
		*hit_distance = CatalystRandomMath::RandomFloatInRange(0.0f, *hit_distance);

		return true;
	}

	else
	{
		return false;
	}
}

/*
*	Casts a ray against terrain.
*/
NO_DISCARD bool RenderingReferenceSystem::CastRayTerrain(const Ray &ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT
{
	//Define constants.
	constexpr float STEP{ 1.0f };

	Vector3<float> current_position{ ray._Origin };
	float current_distance{ 0.0f };

	//Offset the current position by a random amount.
	current_position += ray._Direction * CatalystRandomMath::RandomFloatInRange(0.0f, STEP);

	for (;;)
	{
		//Get the terrain height at the current position.
		float terrain_height;

		TerrainSystem::Instance->GetTerrainHeightAtPosition(current_position, &terrain_height);

		if (current_position._Y <= terrain_height)
		{
			//Fill in the surface description/hit distance.
			surface_description->_Albedo = Vector3<float>(0.0f, 1.0f, 0.0f);
			TerrainSystem::Instance->GetTerrainNormalAtPosition(current_position, &surface_description->_Normal);
			*hit_distance = CatalystRandomMath::RandomFloatInRange(0.0f, *hit_distance);

			return true;
		}

		//Advance the current position.
		current_position += ray._Direction * STEP;
		current_distance += STEP;

		//Abort if the ray has reached the current view distance.
		if (current_distance >= *hit_distance)
		{
			break;
		}
	}

	//Nothing hit.
	return false;
}

/*
*	Casts a ray against the sky. Returns the color.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CastRaySky(const Ray& ray) NOEXCEPT
{
	const Vector3<float> lower_sky_color{ EnvironmentManager::GetLowerSkyColor() };
	const Vector3<float> upper_sky_color{ EnvironmentManager::GetUpperSkyColor() };

	const float upward{ CatalystBaseMath::Maximum<float>(Vector3<float>::DotProduct(ray._Direction, VectorConstants::UP), 0.0f) };

	return CatalystBaseMath::LinearlyInterpolate(lower_sky_color, upper_sky_color, upward);
}

/*
*	Calculates the lighting.
*/
NO_DISCARD Vector3<float> RenderingReferenceSystem::CalculateLighting(const SurfaceDescription& surface_description) NOEXCEPT
{
	return surface_description._Albedo * Vector3<float>::DotProduct(surface_description._Normal, VectorConstants::UP);

}
#endif