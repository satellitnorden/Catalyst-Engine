#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Quaternion.h>

//Path tracing.
#include <PathTracing/Light.h>

namespace PathTracing
{

	class AreaLight final : public PathTracing::Light
	{

	public:

		/*
		*	Default constructor.
		*/
		AreaLight
		(
			const PathTracing::Transform &transform,
			const Vector3<float32> &color,
			const float32 intensity
		) NOEXCEPT;

		/*
		*	Returns the radiance.
		*/
		NO_DISCARD Vector3<float32> Radiance() const NOEXCEPT override;

		/*
		*	Retrieves a random point on this light.
		*/
		NO_DISCARD Vector3<float32> RandomPoint(RandomNumberGenerator *const RESTRICT random_number_generator) const NOEXCEPT override;

	};

}