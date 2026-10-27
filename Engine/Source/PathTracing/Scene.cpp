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
			LOG_INFORMATION("Gathering primitives for path tracing scene...");

			for (const PathTracing::Object *const RESTRICT object : _Objects)
			{
				object->AddPrimitives(&_AccelerationStructure);
			}
		}

		//Build the acceleration structure.
		{
			LOG_INFORMATION("Building acceleration structure for path tracing scene...");
			_AccelerationStructure.Build();
		}

		//Initialize the intermediate render target.
		Texture2D<Vector4<float64>> intermediate_render_target{ resolution._Width, resolution._Height };
		Memory::Set(intermediate_render_target.Data(), 0, resolution._Width * resolution._Height * sizeof(Vector4<float64>));

		//Log the start.
		LOG_INFORMATION("Starting to render path tracing scene...");

		//Render all samples!
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			LOG_INFORMATION("Rendering sample %u of %u", sample_index + 1, number_of_samples);

			for (uint32 Y{ 0 }; Y < resolution._Height; ++Y)
			{
				for (uint32 X{ 0 }; X < resolution._Width; ++X)
				{
					//Calculate the coordinate.
					const Vector2<float32> coordinate
					{
						(static_cast<float32>(X) + _RandomNumberGenerator.RandomFloat<float32>()) / static_cast<float32>(resolution._Width),
						(static_cast<float32>(Y) + _RandomNumberGenerator.RandomFloat<float32>()) / static_cast<float32>(resolution._Height)
					};

					//Generate the ray.
					const Ray ray{ _Camera->GenerateRay(coordinate) };

					//Trace against the acceleration structure.
					PathTracing::SurfaceInformation surface_information;

					if (_AccelerationStructure.TraceSurface(ray, &surface_information))
					{
						//Calculate the offset hit position.
						const Vector3<float32> offset_hit_position{ OffsetHitPosition(surface_information._HitPosition, surface_information._GeometryNormal) };

						//Calculate the ambient occlusion.
						float32 ambient_occlusion{ 1.0f };

						if (surface_information._Material->HasFlag(PathTracing::Material::Flag::RECEIVES_AMBIENT_OCCLUSION))
						{
							//Set up the ray.
							Ray ray;

							//Set the origin.
							ray.SetOrigin(offset_hit_position);

							//Set the dirrection.
							ray.SetDirection(PathTracing::RayGeneration::RandomHemisphereUniform(surface_information._GeometryNormal, &_RandomNumberGenerator));

							//Do the trace!
							if (_AccelerationStructure.TraceOcclusion(ray, _MaximumAmbientOcclusionDistance))
							{
								ambient_occlusion = 0.0f;
							}
						}

						//Calculate the incoming radiance.
						Vector3<float32> outgoing_radiance{ 0.0f, 0.0f, 0.0f };

						//Add all lights.
						if (ambient_occlusion > 0.0f && surface_information._Material->HasFlag(PathTracing::Material::Flag::RECEIVES_LIGHTING))
						{
							for (const PathTracing::Light *const RESTRICT light : _Lights)
							{
								//Sample a random point on the light.
								const Vector3<float32> random_point_on_light{ light->RandomPoint(&_RandomNumberGenerator) };

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
						//TODO: Do this with more clear rules, right now it's hard-coded to a specific case.
						float32 opacity{ 1.0f };

						if (!surface_information._Material->HasFlag(PathTracing::Material::Flag::RECEIVES_LIGHTING)
							&&
							surface_information._Material->HasFlag(PathTracing::Material::Flag::RECEIVES_AMBIENT_OCCLUSION))
						{
							opacity = ambient_occlusion == 0.0f ? 1.0f : 0.0f;
						}

						//Write to the intermediate render target.
						intermediate_render_target.At(X, Y) += Vector4<float64>
						(
							outgoing_radiance._R,
							outgoing_radiance._G,
							outgoing_radiance._B,
							opacity
						);
					}
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
		LOG_INFORMATION("Finished path tracing scene!");

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