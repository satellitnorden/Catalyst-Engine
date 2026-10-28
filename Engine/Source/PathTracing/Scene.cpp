//Header file.
#include <PathTracing/Scene.h>

//Lighting.
#include <Lighting/PhysicallyBasedLighting.h>

//Path tracing.
#include <PathTracing/RayGeneration.h>

//Rendering.
#include <Rendering/Native/Shader/CatalystToneMapping.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/TaskSystem.h>

namespace PathTracing
{

	/*
	*	Default destructor.
	*/
	Scene::~Scene() NOEXCEPT
	{
		//Destroy the camera.
		if (_Camera)
		{
			delete _Camera;
		}

		//Destroy all materials.
		for (PathTracing::Material *const RESTRICT material : _Materials)
		{
			delete material;
		}

		//Destroy all objects.
		for (PathTracing::Object *const RESTRICT object : _Objects)
		{
			delete object;
		}

		//Destroy all lights.
		for (PathTracing::Light *const RESTRICT light : _Lights)
		{
			delete light;
		}
	}

	/*
	*	Returns the bounding box of this scene.
	*/
	NO_DISCARD AxisAlignedBoundingBox3D Scene::BoundingBox() const NOEXCEPT
	{
		AxisAlignedBoundingBox3D bounding_box;

		for (const PathTracing::Object *const RESTRICT object : _Objects)
		{
			bounding_box.Expand(object->BoundingBox());
		}

		return bounding_box;
	}

	/*
	*	Renders this scene.
	*/
	NO_DISCARD Texture2D<Vector4<float32>> Scene::Render
	(
		const Resolution resolution,
		const uint32 number_of_samples
	) NOEXCEPT
	{
		//Sanity check that we are ready for rendering.
		ASSERT(IsReadyForRendering(), "Scene is not ready for rendering!");

		//Gather the primitives from the objects.
		{
			if (_DoLogging) LOG_INFORMATION("Gathering primitives for path tracing scene...");

			for (const PathTracing::Object *const RESTRICT object : _Objects)
			{
				object->AddPrimitives(&_AccelerationStructure);
			}
		}

		//Build the acceleration structure.
		{
			if (_DoLogging) LOG_INFORMATION("Building acceleration structure for path tracing scene...");
			_AccelerationStructure.Build();
		}

		//Initialize the intermediate render target.
		Texture2D<Vector4<float64>> intermediate_render_target{ resolution._Width, resolution._Height };
		Memory::Set(intermediate_render_target.Data(), 0, resolution._Width * resolution._Height * sizeof(Vector4<float64>));

		//Log the start.
		if (_DoLogging) LOG_INFORMATION("Starting to render path tracing scene...");

		//Set up the task data.
		DynamicArray<TaskData> task_data;
		task_data.Upsize<true>(resolution._Height);

		//Render all samples!
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			if (_DoLogging) LOG_INFORMATION("Rendering sample %u of %u", sample_index + 1, number_of_samples);

			for (uint32 Y{ 0 }; Y < resolution._Height; ++Y)
			{
				//Set up the task data.
				TaskData &_task_data{ task_data[Y] };

				_task_data._Scene = this;
				_task_data._Task._Function = [](void *const RESTRICT arguments)
				{
					TaskData *const RESTRICT task_data{ static_cast<TaskData *const RESTRICT>(arguments) };
					task_data->_Scene->RenderTask(*task_data);
				};
				_task_data._Task._Arguments = &_task_data;
				_task_data._Task._ExecutableOnSameThread = false;
				_task_data._Resolution = resolution;
				_task_data._Y = Y;
				_task_data._IntermediateRenderTarget = &intermediate_render_target;
			}

			if (_DoMultithreading)
			{
				//Execute all tasks.
				for (TaskData &_task_data : task_data)
				{
					TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_task_data._Task);
				}

				//Wait for all tasks to finish.
				while (true)
				{
					bool all_tasks_finished{ true };

					for (const TaskData &_task_data : task_data)
					{
						all_tasks_finished = all_tasks_finished && _task_data._Task.IsExecuted();
					}

					if (all_tasks_finished)
					{
						break;
					}

					else
					{
						TaskSystem::Instance->DoWork(Task::Priority::HIGH);
					}
				}
			}

			else
			{
				//Do all the tasks on the current thread.
				for (const TaskData &_task_data : task_data)
				{
					RenderTask(_task_data);
				}
			}

			//Call the partial render callback, if one is set.
			if (_PartialRenderCallback && ShouldCallPartialRenderCallback(sample_index + 1))
			{
				Texture2D<Vector4<float32>> texture;
				AverageRenderTarget(intermediate_render_target, sample_index + 1, &texture);

				_PartialRenderCallback(sample_index + 1, texture, _PartialRenderCallbackUserData);
			}
		}

		//Average into the texture.
		Texture2D<Vector4<float32>> texture;
		AverageRenderTarget(intermediate_render_target, number_of_samples, &texture);

		//Log the end.
		if (_DoLogging) LOG_INFORMATION("Finished path tracing scene!");

		//Return the texture.
		return texture;
	}

	/*
	*	Returns if this scene is ready for rendering.
	*/
	NO_DISCARD bool Scene::IsReadyForRendering() const NOEXCEPT
	{
		//Needs a camera to render.
		if (!_Camera)
		{
			return false;
		}

		//Needs objects to render.
		if (_Objects.Empty())
		{
			return false;
		}

		//All checks passed!
		return true;
	}

	/*
	*	Renders for the given task data.
	*/
	void Scene::RenderTask(const TaskData &task_data) NOEXCEPT
	{
		//TODO: Handle this in a better way?
		static thread_local RandomNumberGenerator RANDOM_NUMBER_GENERATOR;

		//Iterate over the pixels.
		for (uint32 X{ 0 }; X < task_data._Resolution._Width; ++X)
		{
			//Calculate the coordinate.
			const Vector2<float32> coordinate
			{
				(static_cast<float32>(X) + RANDOM_NUMBER_GENERATOR.RandomFloat<float32>()) / static_cast<float32>(task_data._Resolution._Width),
				(static_cast<float32>(task_data._Y) + RANDOM_NUMBER_GENERATOR.RandomFloat<float32>()) / static_cast<float32>(task_data._Resolution._Height)
			};

			//Generate the ray.
			const Ray ray{ _Camera->GenerateRay(coordinate) };

			//Retrieve the outgoing radiance!
			const RadianceResult result{ Radiance(ray, 0) };

			//Add to the intermediate render target if there was a hit.
			if (result._Hit)
			{
				//Write to the intermediate render target.
				task_data._IntermediateRenderTarget->At(X, task_data._Y) += Vector4<float64>
				(
					result._Radiance._R,
					result._Radiance._G,
					result._Radiance._B,
					result._Opacity
				);
			}
		}
	}

	/*
	*	Returns the outgoing radiance in the opposite direction of the given ray.
	*/
	NO_DISCARD Scene::RadianceResult Scene::Radiance(const Ray &ray, const uint32 depth) NOEXCEPT
	{
		//TODO: Handle this in a better way?
		static thread_local RandomNumberGenerator RANDOM_NUMBER_GENERATOR;

		//Terminate if the max depth is reached.
		if (depth >= 2)
		{
			return RadianceResult();
		}

		//Trace against the acceleration structure.
		PathTracing::SurfaceInformation surface_information;

		if (_AccelerationStructure.TraceSurface(ray, &surface_information))
		{
			//Calculate the offset hit position.
			const Vector3<float32> offset_hit_position{ OffsetHitPosition(surface_information._HitPosition, surface_information._GeometryNormal) };

			//Calculate the ambient occlusion.
			float32 ambient_occlusion{ 1.0f };

			//Only the first camera ray gets an ambient occlusion sample, and only if the material is a transparent backdrop.
			if (depth == 0 && surface_information._Material->HasFlag(PathTracing::Material::Flag::TRANSPARENT_BACKDROP))
			{
				//Set up the ray.
				Ray ambient_occlusion_ray;

				//Set the origin.
				ambient_occlusion_ray.SetOrigin(offset_hit_position);

				//Set the dirrection.
				ambient_occlusion_ray.SetDirection(PathTracing::RayGeneration::RandomHemisphereUniform(surface_information._GeometryNormal, &RANDOM_NUMBER_GENERATOR));

				//Do the trace!
				if (_AccelerationStructure.TraceOcclusion(ambient_occlusion_ray, _MaximumAmbientOcclusionDistance))
				{
					ambient_occlusion = 0.0f;
				}
			}

			//Calculate the incoming radiance.
			Vector3<float32> outgoing_radiance{ 0.0f, 0.0f, 0.0f };

			//Add indirect lighting.
			if (!surface_information._Material->HasFlag(PathTracing::Material::Flag::TRANSPARENT_BACKDROP))
			{
				//Set up the ray.
				Ray indirect_lighting_ray;

				//Set the origin.
				indirect_lighting_ray.SetOrigin(offset_hit_position);

				//Set the dirrection.
				indirect_lighting_ray.SetDirection(PathTracing::RayGeneration::RandomHemisphereUniform(surface_information._GeometryNormal, &RANDOM_NUMBER_GENERATOR));

				//Calculate the outgoing radiance along this ray.
				const RadianceResult result{ Radiance(indirect_lighting_ray, depth + 1) };

				//Add it to the outgoing radiance if there was a hit.
				if (result._Hit)
				{
					outgoing_radiance += result._Radiance * PhysicallyBasedLighting::BidirectionalReflectanceDistribution
					(
						-ray._Direction,
						surface_information._Albedo,
						surface_information._ShadingNormal,
						surface_information._Roughness,
						surface_information._Metallic,
						1.0f,
						-indirect_lighting_ray._Direction
					);
				}
			}

			//Add all direct lighting.
			if (!surface_information._Material->HasFlag(PathTracing::Material::Flag::TRANSPARENT_BACKDROP))
			{
				for (const PathTracing::Light *const RESTRICT light : _Lights)
				{
					//Sample a random point on the light.
					const Vector3<float32> random_point_on_light{ light->RandomPoint(&RANDOM_NUMBER_GENERATOR) };

					//Construct the occlusion ray.
					Ray occlusion_ray;

					//Set the origin.
					occlusion_ray.SetOrigin(offset_hit_position);

					//Set the direction.
					occlusion_ray.SetDirection(Vector3<float32>::Normalize(random_point_on_light - offset_hit_position));

					//Cast the occlusion ray.
					if (!_AccelerationStructure.TraceOcclusion(occlusion_ray, FLOAT32_MAXIMUM))
					{
						//Add the outgoing radiance.
						outgoing_radiance += light->Radiance() * PhysicallyBasedLighting::BidirectionalReflectanceDistribution
						(
							-ray._Direction,
							surface_information._Albedo,
							surface_information._ShadingNormal,
							surface_information._Roughness,
							surface_information._Metallic,
							1.0f,
							Vector3<float32>::Normalize(surface_information._HitPosition - random_point_on_light)
						);
					}
				}
			}

			//Calculate the opacity.
			float32 opacity{ 1.0f };

			if (surface_information._Material->HasFlag(PathTracing::Material::Flag::TRANSPARENT_BACKDROP))
			{
				opacity = ambient_occlusion == 0.0f ? 1.0f : 0.0f;
			}

			//Return the radiance result.
			return RadianceResult
			{
				true,
				outgoing_radiance,
				opacity
			};
		}

		else
		{
			//TODO: Sample some ambient color?
			return RadianceResult();
		}
	}

	/*
	*	Offsets the given hit position.
	*/
	NO_DISCARD Vector3<float32> Scene::OffsetHitPosition(const Vector3<float32> &hit_position, const Vector3<float32> &geometry_normal) NOEXCEPT
	{
#if 1
		return hit_position + geometry_normal * FLOAT32_EPSILON * 32.0f;
#else
		Vector3<float32> result{ hit_position };

		for (uint8 axis_index{ 0 }; axis_index < 3; ++axis_index)
		{
			if (geometry_normal[axis_index] > 0.0f)
			{
				result[axis_index] = NextFloat32(result[axis_index]);
			}

			else if (geometry_normal[axis_index] < 0.0f)
			{
				result[axis_index] = PreviousFloat32(result[axis_index]);
			}
		}

		return result;
#endif
	}

	/*
	*	Returns if the partial render callback should be called.
	*/
	NO_DISCARD bool Scene::ShouldCallPartialRenderCallback(const uint32 number_of_samples) NOEXCEPT
	{
		if (number_of_samples < 64)
		{
			return BaseMath::IsPowerOfTwo(number_of_samples);
		}

		else
		{
			return (number_of_samples & (64 - 1)) == 0;
		}
	}

	/*
	*	Averages the given render target into a texture.
	*/
	void Scene::AverageRenderTarget
	(
		const Texture2D<Vector4<float64>> &render_target,
		const uint32 number_of_samples,
		Texture2D<Vector4<float32>> *const RESTRICT texture
	) NOEXCEPT
	{
		//Initialize the texture.
		texture->Initialize(render_target.GetWidth(), render_target.GetHeight());

		//Do the averaging.
		for (uint32 Y{ 0 }; Y < render_target.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < render_target.GetWidth(); ++X)
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					texture->At(X, Y)[i] = static_cast<float32>(render_target.At(X, Y)[i] / static_cast<float64>(number_of_samples));
				}

				Vector3<float32> tonemapped
				{
					texture->At(X, Y)._R,
					texture->At(X, Y)._G,
					texture->At(X, Y)._B
				};

				tonemapped = CatalystToneMapping::ApplyToneMapping(tonemapped);

				texture->At(X, Y)._R = tonemapped._R;
				texture->At(X, Y)._G = tonemapped._G;
				texture->At(X, Y)._B = tonemapped._B;
			}
		}
	}

}