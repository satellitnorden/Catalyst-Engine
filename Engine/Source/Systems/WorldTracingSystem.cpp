//Header file.
#include <Systems/WorldTracingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Shader/CatalystAtmosphericScattering.h>
#include <Rendering/Native/Shader/CatalystLighting.h>

//Systems.
#include <Systems/WorldSystem.h>

//Singleton definition.
DEFINE_SINGLETON(WorldTracingSystem);

//World tracing system constants.
namespace WorldTracingSystemConstants
{
	constexpr float32 DIRECTIONAL_LIGHT_SOFTNESS{ 0.01f };
	constexpr float32 SELF_INTERSECTION_OFFSET{ FLOAT32_EPSILON * 64.0f };
	constexpr uint8 MAXIMUM_RADIANCE_DEPTH{ 2 };
}

/*
*	Caches the world state.
*/
void WorldTracingSystem::CacheWorldState() NOEXCEPT
{
	//Cache the world model state.
	CacheWorldModelState();
}

/*
*	Casts a ray into the world and returns the radiance.
*/
NO_DISCARD Vector3<float32> WorldTracingSystem::RadianceRay(const Ray &ray) NOEXCEPT
{
	return RadianceRayInternal(ray, 0);
}

/*
*	Casts a ray into the world and returns if there was occlusion.
*/
NO_DISCARD bool WorldTracingSystem::OcclusionRay(const Ray &ray) NOEXCEPT
{
	return OcclusionRayModels(ray);
}

/*
*	Caches the world model state.
*/
void WorldTracingSystem::CacheWorldModelState() NOEXCEPT
{
	//Clear the cached model state.
	_CachedModelState.Clear();

	//Cache dynamic models.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Add the new cached model state.
			_CachedModelState.Emplace();
			CachedModelState &cached_model_state{ _CachedModelState.Back() };

			//Cache the world space axis aligned bounding box.
			cached_model_state._WorldSpaceAxisAlignedBoundingBox = component->_WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox();

			//Cache all triangles.
			const Matrix4x4 model_transform{ component->_CurrentWorldTransform.ToAbsoluteMatrix4x4() };

			for (uint64 mesh_index{ 0 }; mesh_index < component->_ModelResource->_Meshes.Size(); ++mesh_index)
			{
				const DynamicArray<Vertex> &vertices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Vertices };
				const DynamicArray<uint32> &indices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Indices };

				for (uint32 triangle_index{ 0 }; triangle_index < indices.Size(); triangle_index += 3)
				{
					cached_model_state._Triangles.Emplace();
					CachedModelState::Triangle &triangle{ cached_model_state._Triangles.Back() };

					triangle._MaterialResource = component->_MaterialResources[mesh_index];

					for (uint8 i{ 0 }; i < 3; ++i)
					{
						triangle._Vertices[i] = vertices[indices[triangle_index + i]];

						triangle._Vertices[i].Transform(model_transform, 0.0f);
					}
				}
			}
		}
	}

	//Cache static models.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
		{
			//Add the new cached model state.
			_CachedModelState.Emplace();
			CachedModelState &cached_model_state{ _CachedModelState.Back() };

			//Cache the world space axis aligned bounding box.
			cached_model_state._WorldSpaceAxisAlignedBoundingBox = component->_WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox();

			//Cache all triangles.
			const Matrix4x4 model_transform{ component->_WorldTransform.ToAbsoluteMatrix4x4() };

			for (uint64 mesh_index{ 0 }; mesh_index < component->_ModelResource->_Meshes.Size(); ++mesh_index)
			{
				const DynamicArray<Vertex> &vertices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Vertices };
				const DynamicArray<uint32> &indices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Indices };

				for (uint32 triangle_index{ 0 }; triangle_index < indices.Size(); triangle_index += 3)
				{
					cached_model_state._Triangles.Emplace();
					CachedModelState::Triangle &triangle{ cached_model_state._Triangles.Back() };

					triangle._MaterialResource = component->_MaterialResources[mesh_index];

					for (uint8 i{ 0 }; i < 3; ++i)
					{
						triangle._Vertices[i] = vertices[indices[triangle_index + i]];

						triangle._Vertices[i].Transform(model_transform, 0.0f);
					}
				}
			}
		}
	}
}

/*
*	Casts a ray into the world and returns the radiance internally.
*/
NO_DISCARD Vector3<float32> WorldTracingSystem::RadianceRayInternal(const Ray &ray, const uint8 depth) NOEXCEPT
{
	//Don't go over the maximum depth.
	if (depth > WorldTracingSystemConstants::MAXIMUM_RADIANCE_DEPTH)
	{
		return Vector3<float32>(0.0f, 0.0f, 0.0f);
	}

	//Remember the closest intersection distance.
	float32 closest_intersection_distance{ FLOAT32_MAXIMUM };

	//Remember the surface description.
	SurfaceDescription surface_description;

	//Remember if anything was hit.
	bool hit_anything{ false };

	//Cast the ray against the surfaces of the world.
	hit_anything |= SurfaceRayModels(ray, &closest_intersection_distance, &surface_description);

	//Did the ray hit anything?
	if (hit_anything)
	{
		//Calculate the hit position.
		const Vector3<float32> hit_position{ ray._Origin + ray._Direction * closest_intersection_distance };

		//Calculate the final radiance.
		Vector3<float32> final_radiance{ 0.0f, 0.0f, 0.0f };

		//Add the emissive lighting.
		{
			final_radiance += surface_description._Albedo * surface_description._Emissive;
		}

		//Add direct lighting.
		{
			const uint64 number_of_components{ ComponentManager::GetNumberOfLightComponents() };
			const LightComponent *RESTRICT component{ ComponentManager::GetLightLightComponents() };

			for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
			{
				switch (component->_LightType)
				{
					case LightType::DIRECTIONAL:
					{
						//Calculate the direction to the light.
						const Vector3<float32> direction_to_light{ Vector3<float32>::Normalize(-CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(component->_Rotation) + CatalystRandomMath::RandomVector3InRange(-WorldTracingSystemConstants::DIRECTIONAL_LIGHT_SOFTNESS, WorldTracingSystemConstants::DIRECTIONAL_LIGHT_SOFTNESS)) };

						//Determine if there is occlusion.
						{
							Ray occlusion_ray;

							occlusion_ray.SetOrigin(hit_position + direction_to_light * WorldTracingSystemConstants::SELF_INTERSECTION_OFFSET);
							occlusion_ray.SetDirection(direction_to_light);

							if (OcclusionRay(occlusion_ray))
							{
								continue;
							}
						}

						//Calculate the lighting.
						final_radiance += CatalystLighting::CalculateLighting(	-ray._Direction,
																				surface_description._Albedo,
																				surface_description._Normal,
																				surface_description._Roughness,
																				surface_description._Metallic,
																				surface_description._AmbientOcclusion,
																				1.0f,
																				-direction_to_light,
																				component->_Color * component->_Intensity);

						break;
					}

					case LightType::POINT:
					{


						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		}

		//Add indirect lighting.
		{
			//Calculate the random hemisphere direction.
			Vector3<float32> random_hemisphere_direction;

			{
				const Vector2<float32> random_hemisphere_coordinate{ CatalystRandomMath::RandomFloat(), CatalystRandomMath::RandomFloat() };
				random_hemisphere_direction = CatalystGeometryMath::CalculateHemisphereUniform(random_hemisphere_coordinate);
			}

			if (Vector3<float32>::DotProduct(surface_description._Normal, random_hemisphere_direction) < 0.0f)
			{
				random_hemisphere_direction *= -1.0f;
			}

			//Construct the indirect ray.
			Ray indirect_ray;

			indirect_ray.SetOrigin(hit_position + random_hemisphere_direction * WorldTracingSystemConstants::SELF_INTERSECTION_OFFSET);
			indirect_ray.SetDirection(random_hemisphere_direction);

			//Cast the ray!
			const Vector3<float32> indirect_radiance{ RadianceRayInternal(indirect_ray, depth + 1) };

			//Add the indirect lighting.
			final_radiance += CatalystLighting::CalculateLighting(	-ray._Direction,
																	surface_description._Albedo,
																	surface_description._Normal,
																	surface_description._Roughness,
																	surface_description._Metallic,
																	surface_description._AmbientOcclusion,
																	1.0f,
																	-random_hemisphere_direction,
																	indirect_radiance);
		}

		//Return the final radiance.
		return final_radiance;
	}

	else
	{
		return SkyRay(ray);
	}
}

/*
*	Casts a surface ray against models.
*/
NO_DISCARD bool WorldTracingSystem::SurfaceRayModels(const Ray &ray, float32 *closest_intersection_distance, SurfaceDescription *const RESTRICT surface_description) NOEXCEPT
{
	//Remember if the ray hit anything.
	bool hit_anything{ false };

	//Remember the hit triangle.
	const CachedModelState::Triangle *RESTRICT hit_triangle{ nullptr };

	//Iterate over all cached models and cast rays against them.
	for (const CachedModelState &cached_model_state : _CachedModelState)
	{
		//First of all, intersect the bounding box, and reject the model completely if it doesn hit.
		{
			float32 intersection_distance;

			if (!CatalystGeometryMath::RayBoxIntersection(ray, cached_model_state._WorldSpaceAxisAlignedBoundingBox, &intersection_distance)
				|| intersection_distance > *closest_intersection_distance)
			{
				continue;
			}
		}

		//Intersect all triangles.
		for (const CachedModelState::Triangle &cached_model_triangle : cached_model_state._Triangles)
		{
			Triangle triangle;

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				triangle._Vertices[i] = cached_model_triangle._Vertices[i]._Position;
			}

			float32 intersection_distance;

			if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &intersection_distance))
			{
				if (*closest_intersection_distance > intersection_distance)
				{
					hit_anything = true;

					*closest_intersection_distance = intersection_distance;

					hit_triangle = &cached_model_triangle;
				}
			}
		}
	}

	//If a triangle was hit, fill in the surface description.
	if (hit_triangle)
	{
		//Calculate the hit point.
		const Vector3<float32> hit_point{ ray._Origin + ray._Direction * *closest_intersection_distance };

		//Calculate the barycentric coordinates.
		Vector3<float32> barycentric_coordinates;

		{
			Triangle triangle;

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				triangle._Vertices[i] = hit_triangle->_Vertices[i]._Position;
			}

			barycentric_coordinates = CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, hit_point);
		}

		//Calculate the texture coordinate.
		Vector2<float32> texture_coordinate{ 0.0f, 0.0f };

		for (uint8 i{ 0 }; i < 3; ++i)
		{
			texture_coordinate += hit_triangle->_Vertices[i]._TextureCoordinate * barycentric_coordinates[i];
		}
		
		//Fill in the albedo.
		switch (hit_triangle->_MaterialResource->_AlbedoThicknessComponent._Type)
		{
			case MaterialResource::MaterialResourceComponent::Type::COLOR:
			{
				const Vector4<float32> color{ hit_triangle->_MaterialResource->_AlbedoThicknessComponent._Color.Get() };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					surface_description->_Albedo[i] = color[i];
				}

				break;
			}

			case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
			{
				const Vector4<float32> color{ hit_triangle->_MaterialResource->_AlbedoThicknessComponent._TextureResource->_Texture2D.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE) };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					surface_description->_Albedo[i] = color[i];
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Fill in the normal.
		{
			//Calculate the tangent space matrix.
			Matrix3x3 tangent_space_matrix;

			{
				Vector3<float32> normal{ 0.0f, 0.0f, 0.0f };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					normal += hit_triangle->_Vertices[i]._Normal * barycentric_coordinates[i];
				}

				Vector3<float32> tangent{ 0.0f, 0.0f, 0.0f };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					tangent += hit_triangle->_Vertices[i]._Tangent * barycentric_coordinates[i];
				}

				const Vector3<float32> bitangent{ Vector3<float32>::CrossProduct(surface_description->_Normal, tangent) };

				tangent_space_matrix = Matrix3x3(tangent, bitangent, surface_description->_Normal);
			}

			//Retrieve the normal map.
			Vector3<float32> normal_map;

			{
				Vector4<float32> normal_map_displacement;

				switch (hit_triangle->_MaterialResource->_NormalMapDisplacementComponent._Type)
				{
					case MaterialResource::MaterialResourceComponent::Type::COLOR:
					{
						normal_map_displacement = hit_triangle->_MaterialResource->_NormalMapDisplacementComponent._Color.Get();

						break;
					}

					case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
					{
						normal_map_displacement = hit_triangle->_MaterialResource->_NormalMapDisplacementComponent._TextureResource->_Texture2D.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				normal_map = Vector3<float32>(normal_map_displacement._X * 2.0f - 1.0f, normal_map_displacement._Y * 2.0f - 1.0f, normal_map_displacement._Z * 2.0f - 1.0f);
			}

			//Fill in the normal.
			surface_description->_Normal = tangent_space_matrix * normal_map;
		}

		//Fill in the material properties.
		Vector4<float32> material_properties;

		switch (hit_triangle->_MaterialResource->_MaterialPropertiesComponent._Type)
		{
			case MaterialResource::MaterialResourceComponent::Type::COLOR:
			{
				material_properties = hit_triangle->_MaterialResource->_MaterialPropertiesComponent._Color.Get();

				break;
			}

			case MaterialResource::MaterialResourceComponent::Type::TEXTURE:
			{
				material_properties = hit_triangle->_MaterialResource->_MaterialPropertiesComponent._TextureResource->_Texture2D.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		surface_description->_Roughness = material_properties[0];
		surface_description->_Metallic = material_properties[1];
		surface_description->_AmbientOcclusion = material_properties[2];
		surface_description->_Emissive = material_properties[3] * hit_triangle->_MaterialResource->_EmissiveMultiplier;
	}

	//Return if the ray hit anything.
	return hit_anything;
}

/*
*	Casts an occlusion ray against models.
*/
NO_DISCARD bool WorldTracingSystem::OcclusionRayModels(const Ray &ray) NOEXCEPT
{
	//Iterate over all cached models and cast rays against them.
	for (const CachedModelState &cached_model_state : _CachedModelState)
	{
		//First of all, intersect the bounding box, and reject the model completely if it doesn't hit.
		if (!CatalystGeometryMath::RayBoxIntersection(ray, cached_model_state._WorldSpaceAxisAlignedBoundingBox, nullptr))
		{
			continue;
		}

		//Intersect all triangles.
		for (const CachedModelState::Triangle &cached_model_triangle : cached_model_state._Triangles)
		{
			Triangle triangle;

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				triangle._Vertices[i] = cached_model_triangle._Vertices[i]._Position;
			}

			float32 intersection_distance;

			if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &intersection_distance))
			{
				return true;
			}
		}
	}

	//Nothing was hit.
	return false;
}

/*
*	Casts a ray against the sky.
*/
NO_DISCARD Vector3<float32> WorldTracingSystem::SkyRay(const Ray &ray) NOEXCEPT
{
	switch (WorldSystem::Instance->GetSkySystem()->GetSkyMode())
	{
		case SkySystem::SkyMode::ATMOSPHERIC_SCATTERING:
		{
			Vector3<float32> sky_light_radiance{ 0.0f, 0.0f, 0.0f };
			Vector3<float32> sky_light_direction{ 0.0f, -1.0f, 0.0f };

			{
				const uint64 number_of_components{ ComponentManager::GetNumberOfLightComponents() };
				const LightComponent *RESTRICT component{ ComponentManager::GetLightLightComponents() };

				for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
				{
					if (component->_LightType == LightType::DIRECTIONAL)
					{
						sky_light_radiance = component->_Color * component->_Intensity;
						sky_light_direction = CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(component->_Rotation);

						break;
					}
				}
			}

			return CatalystAtmosphericScattering::CalculateAtmosphericScattering(ray._Origin, ray._Direction, sky_light_radiance, sky_light_direction);
		}

		case SkySystem::SkyMode::GRADIENT:
		{
			return Vector3<float32>(0.0f, 0.0f, 0.0f);
		}

		case SkySystem::SkyMode::TEXTURE:
		{
			const Vector4<float32> sky_color{ WorldSystem::Instance->GetSkySystem()->GetSkyTexture()->_TextureCube.Sample(ray._Direction) };

			return Vector3<float32>(sky_color._R, sky_color._G, sky_color._B);
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return Vector3<float32>(0.0f, 0.0f, 0.0f);
		}
	}
}