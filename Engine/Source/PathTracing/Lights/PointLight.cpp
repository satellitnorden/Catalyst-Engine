//Header file.
#include <PathTracing/Lights/PointLight.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	PointLight::PointLight
	(
		const PathTracing::Transform &transform,
		const Vector3<float32> &color,
		const float32 intensity
	) NOEXCEPT
		:
		Light(transform, color, intensity)
	{

	}

	/*
	*	Returns the radiance.
	*/
	NO_DISCARD Vector3<float32> PointLight::Radiance() const NOEXCEPT
	{
		return _Color * _Intensity;
	}

	/*
	*	Retrieves a random point on this light.
	*/
	NO_DISCARD Vector3<float32> PointLight::RandomPoint(RandomNumberGenerator *const RESTRICT random_number_generator) const NOEXCEPT
	{
		//Return the random point.
		return _Transform._Position + random_number_generator->RandomPointInSphere() * _Transform._Scale;
	}

}