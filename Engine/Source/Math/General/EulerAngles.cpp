//Header file.
#include <Math/General/EulerAngles.h>

//Math.
#include <Math/General/Quaternion.h>

/*
*	Converts a quaternion to these euler angles.
*/
void EulerAngles::FromQuaternion(const Quaternion &quaternion) NOEXCEPT
{
	//Roll.
	{
		const float32 y{ 2.0f * (quaternion._Y * quaternion._Z + quaternion._W * quaternion._X) };
		const float32 x{ quaternion._W * quaternion._W - quaternion._X * quaternion._X - quaternion._Y * quaternion._Y + quaternion._Z * quaternion._Z };

		if (CatalystBaseMath::Absolute(x) <= FLOAT32_EPSILON
			&& CatalystBaseMath::Absolute(y) <= FLOAT32_EPSILON)
		{
			_Roll = 2.0f * CatalystBaseMath::ArcTangent(quaternion._X, quaternion._W);
		}

		else
		{
			_Roll = CatalystBaseMath::ArcTangent(y, x);
		}
	}

	//Yaw.
	{
		_Yaw = CatalystBaseMath::ArcSine(CatalystBaseMath::Clamp(-2.0f * (quaternion._X * quaternion._Z - quaternion._W * quaternion._Y), -1.0f, 1.0f));
	}

	//Pitch.
	{
		const float32 y{ 2.0f * (quaternion._X * quaternion._Y + quaternion._W * quaternion._Z) };
		const float32 x{ quaternion._W * quaternion._W + quaternion._X * quaternion._X - quaternion._Y * quaternion._Y - quaternion._Z * quaternion._Z };

		if (CatalystBaseMath::Absolute(x) <= FLOAT32_EPSILON
			&& CatalystBaseMath::Absolute(y) <= FLOAT32_EPSILON)
		{
			_Pitch = 0.0f;
		}

		else
		{
			_Pitch = CatalystBaseMath::ArcTangent(y, x);
		}
	}
}