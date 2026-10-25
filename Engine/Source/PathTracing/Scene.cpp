//Header file.
#include <PathTracing/Scene.h>

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

		//Destroy all objects.
		for (PathTracing::Object *const RESTRICT object : _Objects)
		{
			delete object;
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
	NO_DISCARD Texture2D<Vector4<float32>> Scene::Render(const Resolution resolution) NOEXCEPT
	{
		//Sanity check that we are ready for rendering.
		ASSERT(IsReadyForRendering(), "Scene is not ready for rendering!");

		//Log the start.
		LOG_INFORMATION("Starting to render path tracing scene...");

		//Initialize the texture.
		Texture2D<Vector4<float32>> texture{ resolution._Width, resolution._Height };
		
		for (uint32 Y{ 0 }; Y < texture.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < texture.GetWidth(); ++X)
			{
				texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}

		//Calculate the total number of pixels.
		const uint32 total_number_of_pixels{ texture.GetWidth() * texture.GetHeight() };

		//Iterate over the pixels.
		uint32 rendered_pixels{ 0 };
		uint32 last_reported_percent{ 0 };

		for (uint32 Y{ 0 }; Y < texture.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < texture.GetWidth(); ++X)
			{
				//Calculate the coordinate.
				const Vector2<float32> coordinate
				{
					(static_cast<float32>(X) + 0.5f) / static_cast<float32>(texture.GetWidth()),
					(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(texture.GetHeight())
				};

				//Generate the ray.
				const Ray ray{ _Camera->GenerateRay(coordinate) };

				//See if any objects hit.
				PathTracing::SurfaceInformation closest_surface_information;

				for (const PathTracing::Object *const RESTRICT object : _Objects)
				{
					//Check if the ray even hits the bounding box first.
					if (!CatalystGeometryMath::RayBoxIntersection(ray, object->BoundingBox()))
					{
						continue;
					}

					PathTracing::SurfaceInformation surface_information;

					if (object->Trace(ray, &surface_information) && surface_information._HitDistance < closest_surface_information._HitDistance)
					{
						closest_surface_information = surface_information;
					}
				}

				if (closest_surface_information._HitDistance != FLOAT32_MAXIMUM)
				{
					texture.At(X, Y) = Vector4<float32>
					(
						closest_surface_information._Normal._X * 0.5f + 0.5f,
						closest_surface_information._Normal._Y * 0.5f + 0.5f,
						closest_surface_information._Normal._Z * 0.5f + 0.5f,
						1.0f
					);
				}

				//Increment the rendered pixels.
				++rendered_pixels;

				//Calculate the current percent.
				const uint32 current_percent{ BaseMath::Floor<uint32>(static_cast<float32>(rendered_pixels) / static_cast<float32>(total_number_of_pixels) * 100.0f) };

				//Report the progress.
				if (current_percent > last_reported_percent)
				{
					LOG_INFORMATION("%u%%...", current_percent);
					last_reported_percent = current_percent;
				}
			}
		}

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

}