//Header file.
#include <Math/Matrix3.h>

//Math.
#include <Math/Matrix4.h>

/*
*	Default constructor.
*/
Matrix3::Matrix3() NOEXCEPT
	:
	matrix{ {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }
{

}

/*
*	Constructor taking a Matrix4.
*/
Matrix3::Matrix3(const Matrix4 &otherMatrix) NOEXCEPT
	:
	matrix{ { otherMatrix.matrix[0].X, otherMatrix.matrix[0].Y, otherMatrix.matrix[0].Z },{ otherMatrix.matrix[1].X, otherMatrix.matrix[1].Y, otherMatrix.matrix[1].Z },{ otherMatrix.matrix[2].X, otherMatrix.matrix[2].Y, otherMatrix.matrix[2].Z } }
{
	
}

/*
*	Matrix3 by vector3 multiplcation overload.
*/
Vector3 Matrix3::operator*(const Vector3 &vector) NOEXCEPT
{
	Vector3 multipliedVector{ vector };

	multipliedVector.X = (matrix[0].X * vector.X) + (matrix[1].X * vector.Y) + (matrix[2].X * vector.Z);
	multipliedVector.Y = (matrix[0].Y * vector.X) + (matrix[1].Y * vector.Y) + (matrix[2].Y * vector.Z);
	multipliedVector.Z = (matrix[0].Z * vector.X) + (matrix[1].Z * vector.Y) + (matrix[2].Z * vector.Z);

	return multipliedVector;
}