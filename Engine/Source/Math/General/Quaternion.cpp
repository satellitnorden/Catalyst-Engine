//Header file.
#include <Math/General/Quaternion.h>

/*
*	Converts this quaternion to euler angles.
*/
NO_DISCARD EulerAngles Quaternion::ToEulerAngles() const NOEXCEPT
{
	EulerAngles euler_angles;

	euler_angles.FromQuaternion(*this);

	return euler_angles;
}

/*
*	Converts euler angles to this quaternion.
*/
void Quaternion::FromEulerAngles(const EulerAngles &angles) NOEXCEPT
{
	const EulerAngles half_angles
	{
		angles._Roll * 0.5f,
		angles._Yaw * 0.5f,
		angles._Pitch * 0.5f
	};
	const Vector3<float32> cosine
	{
		BaseMath::Cosine(half_angles._Roll),
		BaseMath::Cosine(half_angles._Yaw),
		BaseMath::Cosine(half_angles._Pitch),
	};
	const Vector3<float32> sine
	{
		BaseMath::Sine(half_angles._Roll),
		BaseMath::Sine(half_angles._Yaw),
		BaseMath::Sine(half_angles._Pitch),
	};

#if 1
	_X = sine._X * cosine._Y * cosine._Z - cosine._X * sine._Y * sine._Z;
	_Y = cosine._X * sine._Y * cosine._Z + sine._X * cosine._Y * sine._Z;
	_Z = cosine._X * cosine._Y * sine._Z - sine._X * sine._Y * cosine._Z;
	_W = cosine._X * cosine._Y * cosine._Z + sine._X * sine._Y * sine._Z;
#else
	_X = cosine._X * sine._Y * sine._Z - sine._X * cosine._Y * cosine._Z;
	_Y = -cosine._X * sine._Y * cosine._Z - sine._X * cosine._Y * sine._Z;
	_Z = cosine._X * cosine._Y * sine._Z - sine._X * sine._Y * cosine._Z;
	_W = cosine._X * cosine._Y * cosine._Z + sine._X * sine._Y * sine._Z;
#endif
}