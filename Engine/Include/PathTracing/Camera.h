#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/Ray.h>

namespace PathTracing
{

	/*
	*	Camera class.
	*	Represents a camera in a path tracing context.
	*	Should be overridden by child classes that implements the actual functionality.
	*/
	class Camera
	{

	public:

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Camera() NOEXCEPT
		{

		}

		/*
		*	Sets the aspect ratio of the camera.
		*/
		FORCE_INLINE virtual void SetAspectRatio(const float32 value) NOEXCEPT
		{

		}

		/*
		*	Generates a ray from the given coordinate.
		*/
		virtual NO_DISCARD Ray GenerateRay(const Vector2<float32> coordinate) NOEXCEPT = 0;

	};

}