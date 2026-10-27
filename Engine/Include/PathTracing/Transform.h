#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>
#include <Math/General/Quaternion.h>

namespace PathTracing
{

	class Transform final
	{

	public:

		//The position.
		Vector3<float32> _Position;

		//The rotation.
		Quaternion _Rotation;

		//The scale.
		Vector3<float32> _Scale;

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Transform() NOEXCEPT
			:
			_Position(0.0f, 0.0f, 0.0f),
			_Rotation(),
			_Scale(1.0f, 1.0f, 1.0f)
		{

		}

		/*
		*	Constructor taking all values as arguments.
		*/
		FORCE_INLINE Transform(const Vector3<float32> &position, const Quaternion &rotation, const Vector3<float32> &scale) NOEXCEPT
			:
			_Position(position),
			_Rotation(rotation),
			_Scale(scale)
		{

		}

		/*
		*	Transforms the given position.
		*/
		FORCE_INLINE Vector3<float32> TransformPosition(const Vector3<float32> &position) const NOEXCEPT
		{
			//Calculate the result.
			Vector3<float32> result{ position };

			//Apply scale.
			result *= _Scale;

			//Apply rotation.
			result = _Rotation * result;

			//Apply position.
			result += _Position;

			//Return the result.
			return result;
		}

		/*
		*	Transforms the given direction.
		*/
		FORCE_INLINE Vector3<float32> TransformDirection(const Vector3<float32> &direction) const NOEXCEPT
		{
			//Calculate the result.
			Vector3<float32> result{ direction };

			//Apply scale.
			result *= _Scale;

			//Apply rotation.
			result = _Rotation * result;

			//Normalize the result.
			result.Normalize();

			//Return the result.
			return result;
		}

	};

}