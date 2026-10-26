#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>
#include <Math/General/Vector.h>

namespace PathTracing
{

	/*
	*	Class encapsulating a light.
	*/
	class Light
	{

	public:

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Light(const Vector3<float32> &color, const float32 intensity) NOEXCEPT
			:
			_Color(color),
			_Intensity(intensity)
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE ~Light() NOEXCEPT
		{

		}

		/*
		*	Returns the radiance.
		*/
		FORCE_INLINE virtual NO_DISCARD Vector3<float32> Radiance() const NOEXCEPT
		{
			return _Color * _Intensity;
		}

		/*
		*	Retrieves a random point on this light.
		*/
		virtual NO_DISCARD Vector3<float32> RandomPoint(RandomNumberGenerator *const RESTRICT random_number_generator) const NOEXCEPT = 0;

	protected:

		//The color.
		Vector3<float32> _Color;

		//The intensity.
		float32 _Intensity;

	};

}