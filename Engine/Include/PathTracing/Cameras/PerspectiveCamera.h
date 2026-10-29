#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Path tracing.
#include <PathTracing/Camera.h>
#include <PathTracing/Transform.h>

namespace PathTracing
{

	/*
	*	Perspective camera class.
	*/
	class PerspectiveCamera final : public PathTracing::Camera
	{

	public:

		/*
		*	Default constructor.
		*/
		PerspectiveCamera
		(
			const PathTracing::Transform &transform,
			const float32 field_of_view
		) NOEXCEPT;

		/*
		*	Sets the aspect ratio of the camera.
		*/
		void SetAspectRatio(const float32 value) NOEXCEPT override;

		/*
		*	Generates a ray from the given coordinate.
		*/
		NO_DISCARD Ray GenerateRay(const Vector2<float32> coordinate) NOEXCEPT override;

	private:

		//The transform.
		PathTracing::Transform _Transform;

		//The field of view.
		float32 _FieldOfView;

		//The aspect ratio.
		float32 _AspectRatio;

	};

}