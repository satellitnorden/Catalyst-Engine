//Header file.
#include <PathTracing/Cameras/PerspectiveCamera.h>

//STD.
#include <cmath>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	PerspectiveCamera::PerspectiveCamera
	(
		const PathTracing::Transform &transform,
		const float32 field_of_view
	) NOEXCEPT
		:
		_Transform(transform),
		_FieldOfView(BaseMath::DegreesToRadians(field_of_view * 0.5f))
	{

	}

	/*
	*	Sets the aspect ratio of the camera.
	*/
	void PerspectiveCamera::SetAspectRatio(const float32 value) NOEXCEPT
	{
		_AspectRatio = value;
	}

	/*
	*	Generates a ray from the given coordinate.
	*/
	NO_DISCARD Ray PerspectiveCamera::GenerateRay(const Vector2<float32> coordinate) NOEXCEPT
	{
		//Calculate the half height.
		const float32 half_height{ std::tan(_FieldOfView * 0.5f) };

		//Calculate the half width.
		const float32 half_width{ half_height * _AspectRatio };

		//Calculate the direction.
		Vector3<float32> direction
		{
			(coordinate._X * 2.0f - 1.0f) * half_width,
			(coordinate._Y * 2.0f - 1.0f) * half_height,
			1.0f
		};
		direction.Normalize();
		direction = _Transform._Rotation * direction;

		//Construct the ray.
		Ray ray;

		ray.SetOrigin(_Transform._Position);
		ray.SetDirection(direction);

		//Return the ray.
		return ray;
	}

}