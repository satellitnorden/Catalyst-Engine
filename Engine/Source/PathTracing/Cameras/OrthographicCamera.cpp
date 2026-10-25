//Header file.
#include <PathTracing/Cameras/OrthographicCamera.h>

namespace PathTracing
{

	/*
	*	Generates a ray from the given coordinate.
	*/
	NO_DISCARD Ray OrthographicCamera::GenerateRay(const Vector2<float32> coordinate) NOEXCEPT
	{
		//Generate the origin.
		const Vector3<float32> origin
		{
			BaseMath::LinearlyInterpolate(-0.5f, 0.5f, coordinate._X),
			BaseMath::LinearlyInterpolate(-0.50651f * 0.5f, 0.50651f * 0.5f, coordinate._Y),
			-1.0f
		};

		//Generate the direction.
		const Vector3<float32> direction{ 0.0f, 0.0f, 1.0f };

		//Construct the ray.
		Ray ray;

		ray.SetOrigin(origin);
		ray.SetDirection(direction);

		//Return the ray.
		return ray;
	}

}