//Header file.
#include <Systems/PathTracingSystem.h>

//Components.
#include <Components/Core/Component.h>
#include <Components/Components/LightComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//File.
#include <File/Writers/PNGWriter.h>

//Lighting.
#include <Lighting/PhysicallyBasedLighting.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Path tracing.
#include <PathTracing/PathTracingCore.h>
#include <PathTracing/PathTracingAccelerationStructure.h>

//Rendering.
#include <Rendering/Native/Shader/CatalystToneMapping.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/UserInterfaceSystem.h>
#include <Systems/WorldTracingSystem.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//STL.
#include <stdio.h>

//Constants.
#define NEW_GATHER_TRIANGLES (1)

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

//Path tracing system constants.
namespace PathTracingSystemConstants
{
	constexpr uint8 MAXIMUM_RADIANCE_DEPTH{ 2 };
	constexpr float32 SELF_INTERSECTION_OFFSET{ FLOAT32_EPSILON * 1'024.0f };
}

/*
*	Starts path tracing.
*/
void PathTracingSystem::Start() NOEXCEPT
{
	//Pause the Catalyst engine system.
	CatalystEngineSystem::Instance->SetUpdateSpeed(0.0f);

#if NEW_GATHER_TRIANGLES
	//Gather the path tracing triangles.
	DynamicArray<Vertex> vertices;
	DynamicArray<PathTracingTriangle> triangles;

	Components::GatherPathTracingTriangles(&vertices, &triangles);

	//Allocate the acceleration structure.
	_AccelerationStructure = new PathTracingAccelerationStructure();

	//Add all of the vertices.
	for (const Vertex &vertex : vertices)
	{
		_AccelerationStructure->AddVertex(vertex);
	}

	//Add all of the triangles.
	for (const PathTracingTriangle &triangle : triangles)
	{
		_AccelerationStructure->AddTriangle(triangle);
	}

	//Build!
	_AccelerationStructure->Build(4);
#else
	//Tell the world tracing system to cache the world state.
	WorldTracingSystem::Instance->CacheWorldState();
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

#if NEW_GATHER_TRIANGLES
			radiance = CastRadianceRay(ray, 0);
#else
			{
				//Set up the parameters.
				WorldTracingSystem::RadianceRayParameters parameters;

				parameters._ImprovedRayGeneration = X > 960;

				radiance = WorldTracingSystem::Instance->RadianceRay(ray, &parameters);
			}
#endif

			//Write to the intermediate texture.
			Vector4<float64> &intermediate_sample{ _IntermediateTexture.At(X, queued_row.Get()) };
			intermediate_sample += Vector4<float64>(static_cast<float64>(radiance._R), static_cast<float64>(radiance._G), static_cast<float64>(radiance._B), 1.0);

			//Calculate the final radiance.
			Vector3<float32> final_radiance;

			final_radiance._R = static_cast<float32>(intermediate_sample._R / static_cast<float64>(_CurrentNumberOfSamples));
			final_radiance._G = static_cast<float32>(intermediate_sample._G / static_cast<float64>(_CurrentNumberOfSamples));
			final_radiance._B = static_cast<float32>(intermediate_sample._B / static_cast<float64>(_CurrentNumberOfSamples));

			//Apply tone mapping.
			final_radiance = CatalystToneMapping::ApplyToneMapping(final_radiance);

			//Write to the final texture.
			Vector4<float32> &final_sample{ _FinalTexture.At(X, queued_row.Get()) };

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

/*
*	Casts a radiance ray.
*/
NO_DISCARD Vector3<float32> PathTracingSystem::CastRadianceRay(const Ray &ray, const uint8 depth) NOEXCEPT
{
	//Don't go over the maximum depth.
	if (depth > PathTracingSystemConstants::MAXIMUM_RADIANCE_DEPTH)
	{
		return Vector3<float32>(0.0f, 0.0f, 0.0f);
	}

	//Cast the ray.
	float32 intersection_distance{ FLOAT32_MAXIMUM };
	const PathTracingTriangle *const RESTRICT intersected_triangle{ _AccelerationStructure->TraceSurface(ray, &intersection_distance) };

	if (intersected_triangle)
	{
		//Retrieve the intersected vertices.
		const StaticArray<Vertex, 3> intersected_vertices
		{
			_AccelerationStructure->GetVertex(intersected_triangle->_Indices[0]),
			_AccelerationStructure->GetVertex(intersected_triangle->_Indices[1]),
			_AccelerationStructure->GetVertex(intersected_triangle->_Indices[2])
		};

		//Calculate the hit position.
		const Vector3<float32> hit_position{ ray._Origin + ray._Direction * intersection_distance };

		//Calculate the barycentric coordinates.
		Vector3<float32> barycentric_coordinates;

		{
			Triangle triangle;

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				triangle._Vertices[i] = intersected_vertices[i]._Position;
			}

			barycentric_coordinates = CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, hit_position);
		}

		//Construct the shading context.
		PathTracingShadingContext shading_context;

		shading_context._WorldPosition = hit_position;
		shading_context._GeometryNormal =	intersected_vertices[0]._Normal * barycentric_coordinates[0]
											+ intersected_vertices[1]._Normal * barycentric_coordinates[1]
											+ intersected_vertices[2]._Normal * barycentric_coordinates[2];
		shading_context._GeometryTangent =	intersected_vertices[0]._Tangent * barycentric_coordinates[0]
											+ intersected_vertices[1]._Tangent * barycentric_coordinates[1]
											+ intersected_vertices[2]._Tangent * barycentric_coordinates[2];
		shading_context._TextureCoordinate =	intersected_vertices[0]._TextureCoordinate * barycentric_coordinates[0]
												+ intersected_vertices[1]._TextureCoordinate * barycentric_coordinates[1]
												+ intersected_vertices[2]._TextureCoordinate * barycentric_coordinates[2];
		shading_context._UserData = intersected_triangle->_UserData;

		//Call the shading function!
		PathTracingShadingResult shading_result;
		intersected_triangle->_ShadingFunction(shading_context, &shading_result);

		//Calculate the offset hit position.
		const Vector3<float32> offset_hit_position{ hit_position + shading_context._GeometryNormal * PathTracingSystemConstants::SELF_INTERSECTION_OFFSET };

		//Calculate the final radiance.
		Vector3<float32> final_radiance{ 0.0f, 0.0f, 0.0f };

		//Add emissive lighting.
		final_radiance += shading_result._Albedo * shading_result._Emissive;

		//Add indirect lighting.
		{
			//Generate the irradiance ray.
			Ray irradiance_ray;

			irradiance_ray.SetOrigin(offset_hit_position);
			irradiance_ray.SetDirection(GenerateIrradianceRay(shading_context._GeometryNormal));

			//Cast the ray!
			const Vector3<float32> irradiance{ CastRadianceRay(irradiance_ray, depth + 1) };

			//Add to the final radiance.
			final_radiance += irradiance * PhysicallyBasedLighting::BidirectionalReflectanceDistribution
			(
				-ray._Direction,
				shading_result._Albedo,
				shading_result._ShadingNormal,
				shading_result._Roughness,
				shading_result._Metallic,
				shading_result._Thickness,
				-irradiance_ray._Direction
			);
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

				if (_AccelerationStructure->TraceShadow(occlusion_ray, light_distance))
				{
					continue;
				}
			}

			//Otherwise, add the lighting.
			const Vector3<float32> light_radiance{ instance_data._Color * instance_data._Intensity };

			final_radiance += light_radiance * attenuation * PhysicallyBasedLighting::BidirectionalReflectanceDistribution
			(
				-ray._Direction,
				shading_result._Albedo,
				shading_result._ShadingNormal,
				shading_result._Roughness,
				shading_result._Metallic,
				shading_result._Thickness,
				-direction_to_light
			);
		}

		//Return the final radiance.
		return final_radiance;
	}

	else
	{
		return WorldTracingSystem::Instance->SkyRay(ray);
	}
}

/*
*	Generates an irradiance ray.
*/
NO_DISCARD Vector3<float32> PathTracingSystem::GenerateIrradianceRay(const Vector3<float32> &normal) NOEXCEPT
{
	Vector3<float32> direction{ CatalystRandomMath::RandomPointOnSphere() };

	if (Vector3<float32>::DotProduct(normal, direction) < 0.0f)
	{
		direction *= -1.0f;
	}

	return direction;
}