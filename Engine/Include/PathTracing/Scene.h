#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Path tracing.
#include <PathTracing/Camera.h>
#include <PathTracing/Object.h>

//Rendering.
#include <Rendering/Native/Resolution.h>
#include <Rendering/Native/Texture2D.h>

namespace PathTracing
{

	/*
	*	Scene class.
	*	Holds all of the scene data used to render the final image.
	*/
	class Scene final
	{

	public:

		/*
		*	Default destructor.
		*/
		~Scene() NOEXCEPT;

		/*
		*	Adds the camera.
		*/
		template <typename TYPE, class... Arguments>
		FORCE_INLINE void AddCamera(Arguments&&... arguments) NOEXCEPT
		{
			ASSERT(!_Camera, "Camera already added!");

			_Camera = new TYPE(std::forward<Arguments>(arguments)...);
		}

		/*
		*	Adds an object to the scene - Returns a pointer to the newly created object so that is can be initialized in-place.
		*/
		template <typename TYPE, class... Arguments>
		FORCE_INLINE void AddObject(Arguments&&... arguments) NOEXCEPT
		{
			//Add the object.
			_Objects.Emplace(new TYPE(std::forward<Arguments>(arguments)...));
		}

		/*
		*	Returns the bounding box of this scene.
		*/
		NO_DISCARD AxisAlignedBoundingBox3D BoundingBox() const NOEXCEPT;

		/*
		*	Renders this scene.
		*/
		NO_DISCARD Texture2D<Vector4<float32>> Render(const Resolution resolution) NOEXCEPT;

	private:

		//The camera.
		PathTracing::Camera *RESTRICT _Camera{ nullptr };

		//The objects.
		DynamicArray<PathTracing::Object *RESTRICT> _Objects;

		/*
		*	Returns if this scene is ready for rendering.
		*/
		NO_DISCARD bool IsReadyForRendering() const NOEXCEPT;

	};

}