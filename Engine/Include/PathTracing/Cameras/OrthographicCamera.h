#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Path tracing.
#include <PathTracing/Camera.h>

namespace PathTracing
{

	/*
	*	Orthographic camera class.
	*/
	class OrthographicCamera final : public PathTracing::Camera
	{

	public:

		/*
		*	Generates a ray from the given coordinate.
		*/
		NO_DISCARD Ray GenerateRay(const Vector2<float32> coordinate) NOEXCEPT override;

	};

}