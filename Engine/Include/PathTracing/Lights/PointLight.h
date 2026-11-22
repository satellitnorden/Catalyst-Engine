#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Path tracing.
#include <PathTracing/Light.h>

namespace PathTracing
{

	class PointLight final : public PathTracing::Light
	{

	public:

		/*
		*	Default constructor.
		*/
		PointLight
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