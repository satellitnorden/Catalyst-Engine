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
			const Vector3<float32> &color,
			const float32 intensity,
			const Vector3<float32> &position,
			const Quaternion &rotation,
			const Vector2<float32> size
		) NOEXCEPT;

		/*
		*	Returns the radiance.
		*/
		NO_DISCARD Vector3<float32> Radiance() const NOEXCEPT override;

		/*
		*	Retrieves a random point on this light.
		*/
		NO_DISCARD Vector3<float32> RandomPoint(RandomNumberGenerator *const RESTRICT random_number_generator) const NOEXCEPT override;

	private:

		//The position.
		Vector3<float32> _Position;

		//The rotation.
		Quaternion _Rotation;

		//The size.
		Vector2<float32> _Size;

	};

}