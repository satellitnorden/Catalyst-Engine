#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
//Header file.
#include <Rendering/Native/RenderingReference/RenderingReferenceSystem.h>

//Core.
#include <Core/General/Pair.h>

//Components.
#include <Components/Core/ComponentManager.h>

//File.
#include <File/Writers/TGAWriter.h>

//Managers.
#include <Managers/EnvironmentManager.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/Ray.h>

//Rendering.
#include <Rendering/Native/AccelerationStructure.h>
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/RenderPasses/RenderingReferenceRenderPass.h>
#include <Rendering/Native/Shader/CatalystLighting.h>
#include <Rendering/Native/Shader/CatalystToneMapping.h>

//Resources.
#include <Resources/Loading/ResourceLoader.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UserInterfaceSystem.h>

//Terrin.
#include <Terrain/TerrainGeneralUtilities.h>
#include <Terrain/TerrainVertex.h>

//User interface.
#include <UserInterface/TextUserInterfaceElementDescription.h>

//Rendering reference system constants.
namespace RenderingReferenceSystemConstants
{
	constexpr uint32 SLIZE_SIZE{ 16 };
}

//Rendering reference system data.
namespace RenderingReferenceSystemData
{
	//The terrain acceleration structure.
	AccelerationStructure _TerrainAccelerationStructure;
}

/*
*	Updates the rendering reference system during the render update phase.
*/
void RenderingReferenceSystem::RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//Should the rendering reference start/stop?
	const KeyboardState *const RESTRICT state{ InputSystem::Instance->GetKeyboardState() };

	if (state->GetButtonState(KeyboardButton::F1) == ButtonState::Pressed)
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

	//Calculate how much asynchronous data is needed.
	uint32 current_slice{ 0 };
	const uint32 maximum_y{ RenderingSystem::Instance->GetScaledResolution()._Height };

	uint32 counter{ 0 };

	for (;;)
	{
		++counter;

		current_slice += RenderingReferenceSystemConstants::SLIZE_SIZE;

		if (current_slice >= maximum_y)
		{
			break;
		}
	}

	_AsynchronousData.Reserve(counter);

	//Calculate all the synchronous data.
	current_slice = 0;

	for (;;)
	{
		//Add new asynchronous data.
		_AsynchronousData.Emplace();

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

	//Create the progress information.
	{
		TextUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::Text;
		description._Minimum = Vector2<float>(0.01f, 0.01f);
		description._Maximum = Vector2<float>(0.99f, 0.25f);
		description._Font = &ResourceLoader::GetFont(HashString("Catalyst_Engine_Default_Font"));
		description._Scale = 0.025f;
		description._Text = "";

		_ProgressInformation = static_cast<TextUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}

	//Reset the number of texels calculated.
	_TexelsCalculated.store(0);

	//Prepare the terrain data.
	{
		//Generate the terrain vertices and indices.
		DynamicArray<TerrainVertex> vertices;
		DynamicArray<uint32> indices;

		TerrainGeneralUtilities::GenerateTerrainPlane(	*TerrainSystem::Instance->GetTerrainProperties(),
														&vertices,
														&indices);

		//Iterate over all terrain patches and intersect against their triangles.
		const TerrainPatchInformation* RESTRICT patch_information{ TerrainSystem::Instance->GetTerrainPatchInformations()->Data() };
		const TerrainPatchRenderInformation* RESTRICT patch_render_information{ TerrainSystem::Instance->GetTerrainPatchRenderInformations()->Data() };

		for (uint64 i{ 0 }, size{ TerrainSystem::Instance->GetTerrainPatchInformations()->Size() }; i < size; ++i, ++patch_information, ++patch_render_information)
		{
			//If the axis aligned bounding box was hit, iterate over all the triangles in the terrain plane, transform then and intersect against them.
			for (uint64 j{ 0 }; j < indices.Size(); j += 3)
			{
				//Construct the triangle.
				Triangle triangle;

				triangle._Vertices[0]._X = vertices[indices[j + 0]]._Position._X;
				triangle._Vertices[0]._Y = 0.0f;
				triangle._Vertices[0]._Z = vertices[indices[j + 0]]._Position._Y;

				triangle._Vertices[1]._X = vertices[indices[j + 1]]._Position._X;
				triangle._Vertices[1]._Y = 0.0f;
				triangle._Vertices[1]._Z = vertices[indices[j + 1]]._Position._Y;

				triangle._Vertices[2]._X = vertices[indices[j + 2]]._Position._X;
				triangle._Vertices[2]._Y = 0.0f;
				triangle._Vertices[2]._Z = vertices[indices[j + 2]]._Position._Y;

				triangle._Vertices[0]._X = patch_render_information->_WorldPosition._X + triangle._Vertices[0]._X * patch_render_information->_PatchSize;
				triangle._Vertices[0]._Z = patch_render_information->_WorldPosition._Y + triangle._Vertices[0]._Z * patch_render_information->_PatchSize;

				triangle._Vertices[1]._X = patch_render_information->_WorldPosition._X + triangle._Vertices[1]._X * patch_render_information->_PatchSize;
				triangle._Vertices[1]._Z = patch_render_information->_WorldPosition._Y + triangle._Vertices[1]._Z * patch_render_information->_PatchSize;

				triangle._Vertices[2]._X = patch_render_information->_WorldPosition._X + triangle._Vertices[2]._X * patch_render_information->_PatchSize;
				triangle._Vertices[2]._Z = patch_render_information->_WorldPosition._Y + triangle._Vertices[2]._Z * patch_render_information->_PatchSize;

				TerrainSystem::Instance->GetTerrainHeightAtPosition(triangle._Vertices[0], &triangle._Vertices[0]._Y);
				TerrainSystem::Instance->GetTerrainHeightAtPosition(triangle._Vertices[1], &triangle._Vertices[1]._Y);
				TerrainSystem::Instance->GetTerrainHeightAtPosition(triangle._Vertices[2], &triangle._Vertices[2]._Y);

				//Add the triangle to the terrain acceleration structure.
				RenderingReferenceSystemData::_TerrainAccelerationStructure.AddTriangleData(AccelerationStructure::TriangleData(triangle));
			}
		}
	}

	//Build the terrain acceleration structure.
	RenderingReferenceSystemData::_TerrainAccelerationStructure.Build(16);

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

	//Wait for all tasks to finish.
	for (AsynchronousData& data : _AsynchronousData)
	{
		data._Task.WaitFor();
	}

	//Normalize the values in the rendering reference texture.
	for (uint32 Y{ 0 }; Y < RenderingSystem::Instance->GetScaledResolution()._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < RenderingSystem::Instance->GetScaledResolution()._Width; ++X)
		{
			_RenderingReferenceTexture.At(X, Y) /= static_cast<float>(_Iterations + 1);
		}
	}

	//Write the image to file.
	TGAWriter::Write(_RenderingReferenceTexture, "RenderingReference.tga");

	//Destroy the progress information.
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_ProgressInformation);

	//Set the update speed back to normal.
	CatalystEngineSystem::Instance->SetUpdateSpeed(1.0f);
}

/*
*	Updates the rendering reference.
*/
void RenderingReferenceSystem::UpdateRenderingReference() NOEXCEPT
{
	//Are all tasks executed?
	bool all_tasks_executed{ true };

	for (const AsynchronousData& data : _AsynchronousData)
	{
		all_tasks_executed &= data._Task.IsExecuted();
	}

	//If so, fire off a new batch.
	if (all_tasks_executed)
	{
		//Execute all tasks.
		for (AsynchronousData& data : _AsynchronousData)
		{
			TaskSystem::Instance->ExecuteTask(&data._Task);
		}

		//Update the number of iterations.
		++_Iterations;

		//Reset the texels calculated.
		_TexelsCalculated.store(0);

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

	//Update the progress information.
	const float iteration_percent{ _TexelsCalculated.load() / static_cast<float>(RenderingSystem::Instance->GetScaledResolution()._Width * RenderingSystem::Instance->GetScaledResolution()._Height) * 100.0f };

	char buffer[128];
	sprintf_s(buffer, "Rendering Reference Progress: Iterations - %u - Iteration completion - %.3f%%", _Iterations, iteration_percent);

	_ProgressInformation->_Text = buffer;
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
			//Calculate the texel.
			CalculateTexel(X, Y);

			//Increment the number of texels calculated.
			++_TexelsCalculated;
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
	Vector3<float> color{ CastRayScene(ray, 0) };

	//Perform some post processing...
	color = CatalystToneMapping::ApplyToneMapping(color);

	//Write the color.
	_RenderingReferenceTexture.At(X, Y) += Vector4<float>(color, 1.0f);
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

	//has_hit |= CastSurfaceRayVolumetricParticles(ray, &surface_description, &hit_distance);
	has_hit |= CastSurfaceRayTerrain(ray, &surface_description, &hit_distance);

	//Determine the color.
	if (has_hit)
	{
		return CalculateLighting(ray, hit_distance, surface_description, recursion);
	}

	else
	{
		return CastRaySky(ray);
	}
}

/*
*	Casts a surface ray against the volumetric particles.
*/
NO_DISCARD bool RenderingReferenceSystem::CastSurfaceRayVolumetricParticles(const Ray& ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT
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
*	Casts a surface ray against terrain.
*/
NO_DISCARD bool RenderingReferenceSystem::CastSurfaceRayTerrain(const Ray &ray, SurfaceDescription *const RESTRICT surface_description, float *const RESTRICT hit_distance) NOEXCEPT
{
	//Trace the terrain acceleration structure.
	float intersection_distance{ FLOAT_MAXIMUM };

	if (const AccelerationStructure::TriangleData* const RESTRICT triangle_data{ RenderingReferenceSystemData::_TerrainAccelerationStructure.Trace(ray, &intersection_distance) })
	{
		//Calculate the hit position.
		const Vector3<float> hit_position{ ray._Origin + ray._Direction * intersection_distance };

		//Fill in the surface description.
		surface_description->_Albedo = Vector3<float>(0.0f, 1.0f, 0.0f);
		TerrainSystem::Instance->GetTerrainNormalAtPosition(hit_position, &surface_description->_Normal);

		return true;
	}

	else
	{
		return false;
	}
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
NO_DISCARD Vector3<float> RenderingReferenceSystem::CalculateLighting(const Ray &incoming_ray, const float hit_distance, const SurfaceDescription& surface_description, const uint8 recursion) NOEXCEPT
{
	//Calculate the lighting.
	Vector3<float> lighting{ 0.0f, 0.0f, 0.0f };

	//Calculate the indirect lighting.
	{
		Vector3<float> indirect_lighting_direction;

		indirect_lighting_direction._X = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);
		indirect_lighting_direction._Y = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);
		indirect_lighting_direction._Z = CatalystRandomMath::RandomFloatInRange(-1.0f, 1.0f);

		indirect_lighting_direction.Normalize();

		indirect_lighting_direction = Vector3<float>::DotProduct(surface_description._Normal, indirect_lighting_direction) >= 0.0f ? indirect_lighting_direction : -indirect_lighting_direction;

		Ray indirect_lighting_ray;

		indirect_lighting_ray._Origin = incoming_ray._Origin + incoming_ray._Direction * hit_distance;
		indirect_lighting_ray._Direction = indirect_lighting_direction;
		indirect_lighting_ray._MaximumHitDistance = FLOAT_MAXIMUM;

		Vector3<float> indirect_lighting;

		if (recursion < 1)
		{
			indirect_lighting = CastRayScene(indirect_lighting_ray, recursion + 1);
		}

		else
		{
			indirect_lighting = CastRaySky(indirect_lighting_ray);
		}

		lighting += CatalystLighting::CalculateLighting(-incoming_ray._Direction,
														surface_description._Albedo,
														surface_description._Normal,
														1.0f,
														0.0f,
														1.0f,
														1.0f,
														-indirect_lighting_direction,
														indirect_lighting);
	}

	//Calculate the direct lighting.
	{
		//Iterate over all light components and calculate their lighting.
		const uint64 number_of_light_components{ ComponentManager::GetNumberOfLightComponents() };
		const LightComponent *RESTRICT component{ ComponentManager::GetLightLightComponents() };

		for (uint64 i = 0; i < number_of_light_components; ++i, ++component)
		{
			switch (static_cast<LightType>(component->_LightType))
			{
				case LightType::DIRECTIONAL:
				{
					//Cast a shadow ray to determine visibility.
					Ray shadow_ray;

					shadow_ray._Origin = incoming_ray._Origin + incoming_ray._Direction * hit_distance;
					shadow_ray._Direction = -component->_Direction;
					shadow_ray._MaximumHitDistance = FLOAT_MAXIMUM;

					const bool in_shadow{ CastShadowRayScene(shadow_ray) };

					if (!in_shadow)
					{
						lighting += CatalystLighting::CalculateLighting(-incoming_ray._Direction,
																		surface_description._Albedo,
																		surface_description._Normal,
																		1.0f,
																		0.0f,
																		1.0f,
																		1.0f,
																		component->_Direction,
																		component->_Luminance);
					}

					break;
				}

				case LightType::POINT:
				{
					break;
				}
			}
		}
	}

	return lighting;
}

/*
*	Casts a shadow ray against the scene. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastShadowRayScene(const Ray &ray) NOEXCEPT
{
	//return CastShadowRayTerrain(ray);
	return false;
}

/*
*	Casts a shadow ray against the terrain. Returns if there was a hit.
*/
NO_DISCARD bool RenderingReferenceSystem::CastShadowRayTerrain(const Ray &ray) NOEXCEPT
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
			return true;
		}

		//Advance the current position.
		current_position += ray._Direction * STEP;
		current_distance += STEP;

		//Abort if the ray has reached the current view distance.
		if (current_distance >= 2'048.0f)
		{
			break;
		}
	}

	//Nothing hit.
	return false;
}
#endif