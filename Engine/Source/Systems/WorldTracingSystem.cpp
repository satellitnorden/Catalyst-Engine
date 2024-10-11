//Header file.
#include <Systems/WorldTracingSystem.h>

//Components.
#include <Components/Components/LightComponent.h>
#include <Components/Components/StaticModelComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Lighting.
#include <Lighting/PhysicallyBasedLighting.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Shader/CatalystAtmosphericScattering.h>

//Systems.
#include <Systems/WorldSystem.h>

//World tracing system constants.
namespace WorldTracingSystemConstants
{
	constexpr float32 SELF_INTERSECTION_OFFSET{ FLOAT32_EPSILON * 1'024.0f };
	constexpr uint8 MAXIMUM_RADIANCE_DEPTH{ 2 };
}

//Discard funtions.
class DiscardFunctions final
{

public:

	/*
	*	The masked discard function.
	*/
	FORCE_INLINE static NO_DISCARD bool MaskedDiscardFunction(const AccelerationStructure<WorldTracingSystem::VertexData> &acceleration_structure, const Ray &ray, const uint32 index_1, const uint32 index_2, const uint32 index_3, const float32 intersection_distance) NOEXCEPT
	{
		//Calculate the hit position.
		const Vector3<float32> hit_position{ ray._Origin + ray._Direction * intersection_distance };

		//Retrieve the vertex data.
		StaticArray<AccelerationStructure<WorldTracingSystem::VertexData>::VertexData, 3> vertex_data
		{
			acceleration_structure.GetVertexData(index_1),
			acceleration_structure.GetVertexData(index_2),
			acceleration_structure.GetVertexData(index_3)
		};

		if (vertex_data[0]._UserData._Material->_OpacityComponent._Type == MaterialAsset::Component::Type::COLOR)
		{
			return vertex_data[0]._UserData._Material->_OpacityComponent._Color.Get()[0] >= 0.5f;
		}

		else
		{
			//Calculate the barycentric coordinates.
			Vector3<float32> barycentric_coordinates;

			{
				Triangle triangle;

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					triangle._Vertices[i] = vertex_data[i]._Position;
				}

				barycentric_coordinates = CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, hit_position);
			}

			//Calculate the texture coordinate.
			Vector2<float32> texture_coordinate{ 0.0f, 0.0f };

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				texture_coordinate += vertex_data[i]._UserData._TextureCoordinate * barycentric_coordinates[i];
			}

			const Vector4<float32> color{ SampleConvert(vertex_data[0]._UserData._Material->_OpacityComponent._Texture->_Texture2D, texture_coordinate) };

			return color[0] >= 0.5f;
		}
	}

};

/*
*	Caches the world state.
*/
void WorldTracingSystem::CacheWorldState() NOEXCEPT
{
	//Rememder the index offset.
	uint32 index_offset{ 0 };

	//Add static model triangle and vertex data.
	{
		for (uint64 i{ 0 }; i < StaticModelComponent::Instance->NumberOfInstances(); ++i)
		{
			//Cache the data.
			Entity *const RESTRICT entity{ StaticModelComponent::Instance->InstanceToEntity(i) };
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Cache all triangles.
			const Matrix4x4 model_transform{ world_transform_instance_data._CurrentWorldTransform.ToAbsoluteMatrix4x4() };

			for (uint64 mesh_index{ 0 }; mesh_index < static_model_instance_data._Model->_Meshes.Size(); ++mesh_index)
			{
				const DynamicArray<Vertex> &vertices{ static_model_instance_data._Model->_Meshes[mesh_index]._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[mesh_index]]._Vertices };
				const DynamicArray<uint32> &indices{ static_model_instance_data._Model->_Meshes[mesh_index]._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[mesh_index]]._Indices };

				//Add the triangle data.
				for (uint32 triangle_index{ 0 }; triangle_index < indices.Size(); triangle_index += 3)
				{
					_AccelerationStructure.AddTriangleData(AccelerationStructure<VertexData>::TriangleData(index_offset + indices[triangle_index + 0], index_offset + indices[triangle_index + 1], index_offset + indices[triangle_index + 2], static_model_instance_data._Materials[mesh_index]->_Type == MaterialAsset::Type::MASKED ? DiscardFunctions::MaskedDiscardFunction : nullptr));
				}

				//Add the vertex data.
				for (Vertex vertex : vertices)
				{
					//Transform the vertex.
					vertex.Transform(model_transform, 0.0f);

					//Construct the vertex data.
					VertexData vertex_data;

					vertex_data._Material = static_model_instance_data._Materials[mesh_index];
					vertex_data._Normal = Vector3<float32>::Normalize(vertex._Normal);
					vertex_data._Tangent = Vector3<float32>::Normalize(vertex._Tangent);
					vertex_data._TextureCoordinate = vertex._TextureCoordinate;

					//Add the vertex data!
					_AccelerationStructure.AddVertexData(AccelerationStructure<VertexData>::VertexData(vertex._Position, vertex_data));
				}

				//Update the index offset.
				index_offset += static_cast<uint32>(vertices.Size());
			}
		}
	}

	//Build the acceleration structure.
	_AccelerationStructure.Build(8);
}

/*
*	Returns a description of the surface which the given ray hits. Returns false if no surface was hit.
*/
NO_DISCARD bool WorldTracingSystem::SurfaceDescriptionRay(const Ray &ray, float32 *const RESTRICT intersection_distance, SurfaceDescription *const RESTRICT surface_description) NOEXCEPT
{
	//Cast a ray into the acceleration structure.
	const AccelerationStructure<VertexData>::TriangleData *const RESTRICT triangle_data{ _AccelerationStructure.TraceSurface(ray, intersection_distance) };

	//Did the ray hit anything?
	if (triangle_data)
	{
		//Calculate the hit position.
		const Vector3<float32> hit_position{ ray._Origin + ray._Direction * *intersection_distance };

		//Fill in the surface description.
		{
			//Retrieve the vertex data.
			StaticArray<AccelerationStructure<VertexData>::VertexData, 3> vertex_data
			{
				_AccelerationStructure.GetVertexData(triangle_data->_Indices[0]),
				_AccelerationStructure.GetVertexData(triangle_data->_Indices[1]),
				_AccelerationStructure.GetVertexData(triangle_data->_Indices[2])
			};

			//Calculate the barycentric coordinates.
			Vector3<float32> barycentric_coordinates;

			{
				Triangle triangle;

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					triangle._Vertices[i] = vertex_data[i]._Position;
				}

				barycentric_coordinates = CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, hit_position);
			}

			//Calculate the texture coordinate.
			Vector2<float32> texture_coordinate{ 0.0f, 0.0f };

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				texture_coordinate += vertex_data[i]._UserData._TextureCoordinate * barycentric_coordinates[i];
			}

			//Fill in the albedo.
			switch (vertex_data[0]._UserData._Material->_AlbedoThicknessComponent._Type)
			{
				case MaterialAsset::Component::Type::COLOR:
				{
					const Vector4<float32> color{ vertex_data[0]._UserData._Material->_AlbedoThicknessComponent._Color.Get() };

					for (uint8 i{ 0 }; i < 3; ++i)
					{
						surface_description->_Albedo[i] = color[i];
					}

					break;
				}

				case MaterialAsset::Component::Type::TEXTURE:
				{
					const Vector4<float32> color{ SampleConvert(vertex_data[0]._UserData._Material->_AlbedoThicknessComponent._Texture->_Texture2D, texture_coordinate) };

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
					surface_description->_GeometryNormal = Vector3<float32>(0.0f, 0.0f, 0.0f);

					for (uint8 i{ 0 }; i < 3; ++i)
					{
						surface_description->_GeometryNormal += vertex_data[i]._UserData._Normal * barycentric_coordinates[i];
					}

					Vector3<float32> tangent{ 0.0f, 0.0f, 0.0f };

					for (uint8 i{ 0 }; i < 3; ++i)
					{
						tangent += vertex_data[i]._UserData._Tangent * barycentric_coordinates[i];
					}

					const Vector3<float32> bitangent{ Vector3<float32>::CrossProduct(surface_description->_GeometryNormal, tangent) };

					tangent_space_matrix = Matrix3x3(tangent, bitangent, surface_description->_GeometryNormal);
				}

				//Retrieve the normal map.
				Vector3<float32> normal_map;

				{
					Vector4<float32> normal_map_displacement;

					switch (vertex_data[0]._UserData._Material->_NormalMapDisplacementComponent._Type)
					{
						case MaterialAsset::Component::Type::COLOR:
						{
							normal_map_displacement = vertex_data[0]._UserData._Material->_NormalMapDisplacementComponent._Color.Get();

							break;
						}

						case MaterialAsset::Component::Type::TEXTURE:
						{
							normal_map_displacement = SampleConvert(vertex_data[0]._UserData._Material->_NormalMapDisplacementComponent._Texture->_Texture2D, texture_coordinate);

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

				//Fill in the shading normal.
				surface_description->_ShadingNormal = tangent_space_matrix * normal_map;
			}

			//Fill in the material properties.
			Vector4<float32> material_properties;

			switch (vertex_data[0]._UserData._Material->_MaterialPropertiesComponent._Type)
			{
				case MaterialAsset::Component::Type::COLOR:
				{
					material_properties = vertex_data[0]._UserData._Material->_MaterialPropertiesComponent._Color.Get();

					break;
				}

				case MaterialAsset::Component::Type::TEXTURE:
				{
					material_properties = SampleConvert(vertex_data[0]._UserData._Material->_MaterialPropertiesComponent._Texture->_Texture2D, texture_coordinate);

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
			surface_description->_Emissive = material_properties[3] * vertex_data[0]._UserData._Material->_EmissiveMultiplier;
		}

		return true;
	}

	else
	{
		return false;
	}
}

/*
*	Casts a ray into the world and returns the radiance.
*/
NO_DISCARD Vector3<float32> WorldTracingSystem::RadianceRay(const Ray &ray, const RadianceRayParameters *const RESTRICT parameters) NOEXCEPT
{
	return RadianceRayInternal(ray, parameters, 0);
}

/*
*	Casts a ray into the world and returns if there was occlusion.
*/
NO_DISCARD bool WorldTracingSystem::OcclusionRay(const Ray &ray, const float32 length) NOEXCEPT
{
	return _AccelerationStructure.TraceShadow(ray, length);
}

/*
*	Casts a ray into the world and checks for the hit distance. Returns if there was a hit.
*/
NO_DISCARD bool WorldTracingSystem::DistanceRay(const Ray &ray, const float32 maximum_distance, float32 *const RESTRICT hit_distance, const bool use_cached_world_state) NOEXCEPT
{
	//Set the hit distance to maximum distance, to early rejection.
	*hit_distance = maximum_distance;

	return DistanceRayModels(ray, maximum_distance, hit_distance, use_cached_world_state);
}

/*
*	Casts a ray against the sky.
*/
NO_DISCARD Vector3<float32> WorldTracingSystem::SkyRay(const Ray& ray) NOEXCEPT
{
	switch (WorldSystem::Instance->GetSkySystem()->GetSkyMode())
	{
		case SkySystem::SkyMode::ATMOSPHERIC_SCATTERING:
		{
			Vector3<float32> sky_light_radiance{ 0.0f, 0.0f, 0.0f };
			Vector3<float32> sky_light_direction{ 0.0f, -1.0f, 0.0f };

			/*
			{
				const uint64 number_of_components{ ComponentManager::GetNumberOfLightComponents() };
				const LightComponent* RESTRICT component{ ComponentManager::GetLightLightComponents() };

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
			*/

			return CatalystAtmosphericScattering::CalculateAtmosphericScattering(ray._Origin, ray._Direction, sky_light_radiance, sky_light_direction) * WorldSystem::Instance->GetSkySystem()->GetSkyIntensity();
		}

		case SkySystem::SkyMode::GRADIENT:
		{
			const SkyGradient &sky_gradient{ WorldSystem::Instance->GetSkySystem()->GetSkyGradient() };
			const float32 up_factor{ Vector3<float32>::DotProduct(ray._Direction, Vector3<float32>(0.0f, 1.0f, 0.0f)) * 0.5f + 0.5f };

			return BaseMath::LinearlyInterpolate(sky_gradient._LowerSkyColor, sky_gradient._UpperSkyColor, up_factor) * WorldSystem::Instance->GetSkySystem()->GetSkyIntensity();
		}

		case SkySystem::SkyMode::TEXTURE:
		{
			const Vector4<float32> sky_color{ WorldSystem::Instance->GetSkySystem()->GetSkyTexture()->_TextureCube.Sample(ray._Direction) };

			return Vector3<float32>(sky_color._R, sky_color._G, sky_color._B) * WorldSystem::Instance->GetSkySystem()->GetSkyIntensity();
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return Vector3<float32>(0.0f, 0.0f, 0.0f);
		}
	}
}

/*
*	Casts a ray into the world and returns the radiance internally.
*/
NO_DISCARD Vector3<float32> WorldTracingSystem::RadianceRayInternal(const Ray &ray, const RadianceRayParameters *const RESTRICT parameters, const uint8 depth) NOEXCEPT
{
	//Don't go over the maximum depth.
	if (depth > WorldTracingSystemConstants::MAXIMUM_RADIANCE_DEPTH)
	{
		return Vector3<float32>(0.0f, 0.0f, 0.0f);
	}

	//Retrieve the surface description.
	float32 intersection_distance{ FLOAT32_MAXIMUM };
	SurfaceDescription surface_description;
	bool could_get_surface_description;

	{
		PROFILING_SCOPE("WorldTracingSystem::RadianceRayInternal::SurfaceDescriptionRay");

		could_get_surface_description = SurfaceDescriptionRay(ray, &intersection_distance, &surface_description);
	}

	//Did the ray hit anything?
	if (could_get_surface_description)
	{
		//Calculate the hit position.
		const Vector3<float32> hit_position{ ray._Origin + ray._Direction * intersection_distance };

		//Calculate the offset hit position.
		const Vector3<float32> offset_hit_position{ hit_position + surface_description._GeometryNormal * WorldTracingSystemConstants::SELF_INTERSECTION_OFFSET };

		//Calculate the final radiance.
		Vector3<float32> final_radiance{ 0.0f, 0.0f, 0.0f };

		//Add the emissive lighting.
		{
			final_radiance += surface_description._Albedo * surface_description._Emissive;
		}

		//Add direct lighting.
		for (uint64 instance_index{ 0 }; instance_index < LightComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			//Cache the instance data.
			const LightInstanceData &instance_data{ LightComponent::Instance->InstanceData()[instance_index] };

			//Calculate the direction to the light and the distance.
			Vector3<float32> direction_to_light;
			float32 light_distance;
			float32 attenuation{ 1.0f };

			switch (instance_data._LightType)
			{
				case LightType::DIRECTIONAL:
				{
					//Calculate the direction.
					direction_to_light = -CatalystCoordinateSpacesUtilities::RotatedWorldDownVector(instance_data._DirectionalLightData._Rotation);

					/*
					*   This assumes the directional light is representing a sun.
					*   The sun is on average 150 million kilometers away from the earth,
					*   with a radius of 696 340 kilometers.
					*   If we scale those values down a bit for floating point precision's sake,
					*   we can imagine a sphere offset in the directional light direction from the origin,
					*   and grab a random point in that sphere.
					*/
					Vector3<float32> random_point_on_sphere{ CatalystRandomMath::RandomPointOnSphere() };

					if (Vector3<float32>::DotProduct(direction_to_light, random_point_on_sphere) >= 0.0f)
					{
						random_point_on_sphere = -random_point_on_sphere;
					}

					random_point_on_sphere *= 0.0046422666666667f;

					direction_to_light = Vector3<float32>::Normalize(direction_to_light + random_point_on_sphere);

					//Set the light distance.
					light_distance = FLOAT32_MAXIMUM;

					break;
				}

				case LightType::POINT:
				{
					//Cache the world transform instance data.
					const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(LightComponent::Instance->InstanceToEntity(instance_index)) };

					//Cache the light position.
					Vector3<float32> light_position{ world_transform_instance_data._CurrentWorldTransform.GetAbsolutePosition() };

					//Randomize the position a bit. (:
					light_position += CatalystRandomMath::RandomPointOnSphere(0.25f);

					//Calculate the distance to the light.
					const float32 distance_to_light{ Vector3<float32>::Length(light_position - hit_position) };

					//Calculate the direction to the light.
					direction_to_light = (light_position - hit_position) * (1.0f / distance_to_light);

					//Set the light distance.
					light_distance = distance_to_light;

					//Set the attenuation.
					attenuation = 1.0f / distance_to_light;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Determine if there is occlusion.
			{
				Ray occlusion_ray;

				occlusion_ray.SetOrigin(offset_hit_position);
				occlusion_ray.SetDirection(direction_to_light);

				if (OcclusionRay(occlusion_ray, light_distance))
				{
					continue;
				}
			}

			//Otherwise, add the lighting.
			const Vector3<float32> light_radiance{ instance_data._Color * instance_data._Intensity };

			final_radiance += light_radiance * attenuation * PhysicallyBasedLighting::BidirectionalReflectanceDistribution
			(
				-ray._Direction,
				surface_description._Albedo,
				surface_description._ShadingNormal,
				surface_description._Roughness,
				surface_description._Metallic,
				1.0f,
				-direction_to_light
			);
		}

		//Add indirect lighting.
		{
			//Calculate the indirect lighting direction.
			Vector3<float32> indirect_lighting_direction;
			float32 indirect_lighting_probability_density;

			GenerateIrradianceRay
			(
				ray._Direction,
				surface_description._ShadingNormal,
				surface_description._Roughness,
				surface_description._Metallic,
				&indirect_lighting_direction,
				&indirect_lighting_probability_density,
				parameters && parameters->_ImprovedRayGeneration
			);

			if (indirect_lighting_probability_density <= 0.0f)
			{
				return Vector3<float32>(0.0f, 0.0f, 0.0f);
			}

			//Construct the indirect ray.
			Ray indirect_ray;

			indirect_ray.SetOrigin(offset_hit_position);
			indirect_ray.SetDirection(indirect_lighting_direction);

			//Cast the ray!
			Vector3<float32> indirect_radiance;

			{
				PROFILING_SCOPE("WorldTracingSystem::RadianceRayInternal::RadianceRayInternal");

				indirect_radiance = RadianceRayInternal(indirect_ray, parameters, depth + 1);
			}

			//Normalize.
			const float32 indirect_lighting_probability_density_reciprocal{ 1.0f / indirect_lighting_probability_density };
			indirect_radiance *= indirect_lighting_probability_density_reciprocal;

			//Add the indirect lighting.
			{
				PROFILING_SCOPE("WorldTracingSystem::RadianceRayInternal::CalculateLighting");

				final_radiance += indirect_radiance * PhysicallyBasedLighting::BidirectionalReflectanceDistribution
				(
					-ray._Direction,
					surface_description._Albedo,
					surface_description._ShadingNormal,
					surface_description._Roughness,
					surface_description._Metallic,
					1.0f,
					-indirect_lighting_direction
				);
			}
		}

		//Return the final radiance.
		return final_radiance;
	}

	else
	{
		PROFILING_SCOPE("WorldTracingSystem::RadianceRayInternal::SkyRay");

		return SkyRay(ray);
	}
}

/*
*	Casts an distance ray against models.
*/
NO_DISCARD bool WorldTracingSystem::DistanceRayModels(const Ray &ray, const float32 maximum_distance, float32 *const RESTRICT hit_distance, const bool use_cached_world_state) NOEXCEPT
{
	if (use_cached_world_state)
	{
		//Cast a ray into the acceleration structure.
		const AccelerationStructure<VertexData>::TriangleData *const RESTRICT triangle_data{ _AccelerationStructure.TraceSurface(ray, hit_distance) };

		return triangle_data != nullptr;
	}

	else
	{
		//Remember if there was a hit.
		bool hit{ false };

		/*
		//Go through dynamic models.
		{
			const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
			DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

			for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
			{
				//Cache the world space axis aligned bounding box.
				const AxisAlignedBoundingBox3D axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox() };

				//Intersect the bounding box, and reject the model completely if it doesn't hit.
				float32 box_intersection_distance;

				if (!CatalystGeometryMath::RayBoxIntersection(ray, axis_aligned_bounding_box, &box_intersection_distance))
				{
					continue;
				}

				else if (box_intersection_distance >= *hit_distance)
				{
					continue;
				}

				//Intersect all triangles.
				const Matrix4x4 model_transform{ component->_CurrentWorldTransform.ToAbsoluteMatrix4x4() };

				for (uint64 mesh_index{ 0 }; mesh_index < component->_ModelResource->_Meshes.Size(); ++mesh_index)
				{
					const DynamicArray<Vertex> &vertices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Vertices };
					const DynamicArray<uint32> &indices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Indices };

					for (uint32 triangle_index{ 0 }; triangle_index < indices.Size(); triangle_index += 3)
					{
						Triangle triangle;

						for (uint8 i{ 0 }; i < 3; ++i)
						{
							const Vector4<float32> transformed_triangle{ model_transform * Vector4<float32>(vertices[indices[triangle_index + i]]._Position._X, vertices[indices[triangle_index + i]]._Position._Y, vertices[indices[triangle_index + i]]._Position._Z, 1.0f) };

							for (uint8 j{ 0 }; j < 3; ++j)
							{
								triangle._Vertices[i][j] = transformed_triangle[j];
							}
						}

						float32 triangle_intersection_distance;

						if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &triangle_intersection_distance)
							&& *hit_distance > triangle_intersection_distance)
						{
							*hit_distance = triangle_intersection_distance;

							hit = true;
						}
					}
				}
			}
		}
		*/

		/*
		//Go through static models.
		{
			const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
			StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

			for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
			{
				//Cache the world space axis aligned bounding box.
				const AxisAlignedBoundingBox3D axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetAbsoluteAxisAlignedBoundingBox() };

				//Intersect the bounding box, and reject the model completely if it doesn't hit.
				float32 box_intersection_distance;

				if (!CatalystGeometryMath::RayBoxIntersection(ray, axis_aligned_bounding_box, &box_intersection_distance))
				{
					continue;
				}

				else if (box_intersection_distance >= *hit_distance)
				{
					continue;
				}

				//Intersect all triangles.
				const Matrix4x4 model_transform{ component->_WorldTransform.ToAbsoluteMatrix4x4() };

				for (uint64 mesh_index{ 0 }; mesh_index < component->_ModelResource->_Meshes.Size(); ++mesh_index)
				{
					const DynamicArray<Vertex> &vertices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Vertices };
					const DynamicArray<uint32> &indices{ component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[component->_LevelOfDetailIndices[mesh_index]]._Indices };

					for (uint32 triangle_index{ 0 }; triangle_index < indices.Size(); triangle_index += 3)
					{
						Triangle triangle;

						for (uint8 i{ 0 }; i < 3; ++i)
						{
							const Vector4<float32> transformed_triangle{ model_transform * Vector4<float32>(vertices[indices[triangle_index + i]]._Position._X, vertices[indices[triangle_index + i]]._Position._Y, vertices[indices[triangle_index + i]]._Position._Z, 1.0f) };

							for (uint8 j{ 0 }; j < 3; ++j)
							{
								triangle._Vertices[i][j] = transformed_triangle[j];
							}
						}

						float32 triangle_intersection_distance;

						if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &triangle_intersection_distance)
							&& *hit_distance > triangle_intersection_distance)
						{
							*hit_distance = triangle_intersection_distance;

							hit = true;
						}
					}
				}
			}
		}
		*/

		//Return if there was a hit.
		return hit;
	}
}

/*
*	The probability density function.
*/
NO_DISCARD float32 WorldTracingSystem::ProbabilityDensityFunction
(
	const Vector3<float32> &view_direction,
	const Vector3<float32> &normal,
	const float32 roughness,
	const float32 metallic,
	const Vector3<float32> &direction
) NOEXCEPT
{
	//Consider the BRDF.
	const float32 BRDF
	{
		PhysicallyBasedLighting::BidirectionalReflectanceDistribution
		(
			-view_direction,
			Vector3<float32>(1.0f, 1.0f, 1.0f),
			normal,
			roughness,
			metallic,
			1.0f,
			-direction
		)._X
	};

	//Combine the terms.
	return BRDF;
}

/*
*	Generates an irradiance ray.
*/
void WorldTracingSystem::GenerateIrradianceRay
(
	const Vector3<float32> &view_direction,
	const Vector3<float32> &normal,
	const float32 roughness,
	const float32 metallic,
	Vector3<float32> *const RESTRICT direction,
	float32 *const RESTRICT probability_density,
	const bool improved_ray_generation
) NOEXCEPT
{
	PROFILING_SCOPE("WorldTracingSystem::GenerateIrradianceRay");

	/*
	*	This is quite crude, but let's do it like this until I have figured out a better way to do it.
	*	Generate N random direction, then randomly pick one of them weighted based on the probability density function.
	*/
	constexpr uint64 NUMBER_OF_SAMPLES{ 2 };

	StaticArray<Vector3<float32>, NUMBER_OF_SAMPLES> directions;
	StaticArray<float32, NUMBER_OF_SAMPLES> weights;

	for (uint64 i{ 0 }; i < NUMBER_OF_SAMPLES; ++i)
	{
		Vector3<float32> diffuse_direction{ CatalystRandomMath::RandomPointOnSphere() };

		if (Vector3<float32>::DotProduct(normal, diffuse_direction) < 0.0f)
		{
			diffuse_direction *= -1.0f;
		}

		const Vector3<float32> specular_direction{ Vector3<float32>::Reflect(view_direction, normal) };

		const Vector3<float32> final_direction{ Vector3<float32>::Normalize(BaseMath::LinearlyInterpolate(specular_direction, diffuse_direction, roughness)) };

		directions[i] = final_direction;
		weights[i] = ProbabilityDensityFunction(view_direction, normal, roughness, metallic, final_direction);
	}

	ArrayProxy<Vector3<float32>> directions_proxy{ directions };
	ArrayProxy<float32> weights_proxy{ weights };

	const uint64 chosen_index{ CatalystRandomMath::WeightedRandomIndex(directions_proxy, weights_proxy) };

	*direction = directions[chosen_index];
	*probability_density = weights[chosen_index];
}

/*
*	Returns the ambient occlusion at the given position.
*/
NO_DISCARD float32 WorldTracingSystem::SampleAmbientOcclusion(const Vector3<float32> &position, const Vector3<float32> &normal) NOEXCEPT
{
	//Construct the ray direction.
	Vector3<float32> ray_direction{ CatalystRandomMath::RandomPointOnSphere() };

	if (Vector3<float32>::DotProduct(normal, ray_direction) < 0.0f)
	{
		ray_direction *= -1.0f;
	}

	//Construct the ray.
	Ray ray;

	ray.SetOrigin(position);
	ray.SetDirection(ray_direction);

	//Cast the ray!
	float32 hit_distance;
	const bool hit{ DistanceRay(ray, 2.0f, &hit_distance) };

	//Return the ambient occlusion.
	return hit ? hit_distance / 2.0f : 1.0f;
}