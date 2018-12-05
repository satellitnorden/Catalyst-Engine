//Header file.
#include <Math/Matrix3.h>

//Math.
#include <Math/Matrix4.h>

/*
*	Default constructor.
*/
Matrix3::Matrix3() NOEXCEPT
	:
	_Matrix{ {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }
{

}

/*
*	Constructor taking a Matrix4.
*/
Matrix3::Matrix3(const Matrix4 &otherMatrix) NOEXCEPT
	:
	_Matrix{ { otherMatrix._Matrix[0]._X, otherMatrix._Matrix[0]._Y, otherMatrix._Matrix[0]._Z },{ otherMatrix._Matrix[1]._X, otherMatrix._Matrix[1]._Y, otherMatrix._Matrix[1]._Z },{ otherMatrix._Matrix[2]._X, otherMatrix._Matrix[2]._Y, otherMatrix._Matrix[2]._Z } }
{
	
}

/*
*	Constructor taking the three vectors as arguments.
*/
Matrix3::Matrix3(const Vector3<float> &first, const Vector3<float> &second, const Vector3<float> &third) NOEXCEPT
	:
	_Matrix{ first, second, third }
{

}

/*
*	Matrix3 by vector3 multiplcation overload.
*/
Vector3<float> Matrix3::operator*(const Vector3<float> &vector) const NOEXCEPT
{
		Vector3<float> multipliedVector{ vector };

	multipliedVector._X = (_Matrix[0]._X * vector._X) + (_Matrix[1]._X * vector._Y) + (_Matrix[2]._X * vector._Z);
	multipliedVector._Y = (_Matrix[0]._Y * vector._X) + (_Matrix[1]._Y * vector._Y) + (_Matrix[2]._Y * vector._Z);
	multipliedVector._Z = (_Matrix[0]._Z * vector._X) + (_Matrix[1]._Z * vector._Y) + (_Matrix[2]._Z * vector._Z);

	return multipliedVector;
}