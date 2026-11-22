//Header file.
#include <PathTracing/Lights/AreaLight.h>

namespace PathTracing
{

	/*
	*	Default constructor.
	*/
	AreaLight::AreaLight
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
	NO_DISCARD Vector3<float32> AreaLight::Radiance() const NOEXCEPT
	{
		//TODO: Figure out how to normalize the intensity here.
		return _Color * _Intensity;
	}

	/*
	*	Retrieves a random point on this light.
	*/
	NO_DISCARD Vector3<float32> AreaLight::RandomPoint(RandomNumberGenerator *const RESTRICT random_number_generator) const NOEXCEPT
	{
		//Calculate the offset.
		Vector3<float32> offset
		{
			BaseMath::LinearlyInterpolate(-_Transform._Scale._X * 0.5f, _Transform._Scale._X * 0.5f, random_number_generator->RandomFloat<float32>()),
			BaseMath::LinearlyInterpolate(-_Transform._Scale._Y * 0.5f, _Transform._Scale._Y * 0.5f, random_number_generator->RandomFloat<float32>()),
			0.0f
		};

		offset = _Transform._Rotation * offset;

		//Return the random point.
		return _Transform._Position + offset;
	}

}