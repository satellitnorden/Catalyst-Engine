//Header file.
#include <PathTracing/Cameras/OrthographicCamera.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	OrthographicCamera::OrthographicCamera
	(
		const Vector3<float32> &position,
		const Vector3<float32> &direction,
		const Vector2<float32> bounds
	) NOEXCEPT
		:
		_Position(position),
		_Direction(direction),
		_Bounds(bounds)
	{

	}

	/*
	*	Generates a ray from the given coordinate.
	*/
	NO_DISCARD Ray OrthographicCamera::GenerateRay(const Vector2<float32> coordinate) NOEXCEPT
	{
		//Generate the origin.
		//TODO: Account for the provided direction here. This should do for now, assuming a strictly forward pointing direction.
		const Vector3<float32> origin
		{
			_Position._X + BaseMath::LinearlyInterpolate(-_Bounds._X * 0.5f, _Bounds._X * 0.5f, coordinate._X),
			_Position._Y + BaseMath::LinearlyInterpolate(-_Bounds._Y * 0.5f, _Bounds._Y * 0.5f, coordinate._Y),
			_Position._Z
		};

		//Construct the ray.
		Ray ray;

		ray.SetOrigin(origin);
		ray.SetDirection(_Direction);

		//Return the ray.
		return ray;
	}

}