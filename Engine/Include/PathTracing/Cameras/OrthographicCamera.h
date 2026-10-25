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
		*	Default constructor.
		*/
		OrthographicCamera
		(
			const Vector3<float32> &position,
			const Vector3<float32> &direction,
			const Vector2<float32> bounds
		) NOEXCEPT;

		/*
		*	Generates a ray from the given coordinate.
		*/
		NO_DISCARD Ray GenerateRay(const Vector2<float32> coordinate) NOEXCEPT override;

	private:

		//The position.
		Vector3<float32> _Position;

		//The direction.
		Vector3<float32> _Direction;

		//The bounds.
		Vector2<float32> _Bounds;

	};

}