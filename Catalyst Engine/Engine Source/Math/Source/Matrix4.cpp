//Header file.
#include <Matrix4.h>

//Math.
#include <GameMath.h>
#include <Matrix3.h>
#include <Vector3.h>

/*
*	Calculates a look at matrix.
*/
Matrix4 Matrix4::LookAt(const Vector3 &position, const Vector3 &direction, const Vector3 &up) CATALYST_NOEXCEPT
{
	Vector3 F{ Vector3::Normalize(direction - position) };
	Vector3 S{ Vector3::CrossProduct(F, up) };
	Vector3 U{ Vector3::CrossProduct(S, F) };

	Matrix4 result;

	result.matrix[0].X = S.X;
	result.matrix[1].X = S.Y;
	result.matrix[2].X = S.Z;

	result.matrix[0].Y = U.X;
	result.matrix[1].Y = U.Y;
	result.matrix[2].Y = U.Z;

	result.matrix[0].Z = -F.X;
	result.matrix[1].Z = -F.Y;
	result.matrix[2].Z = -F.Z;

	result.matrix[3].X = -Vector3::DotProduct(S, position);
	result.matrix[3].Y = -Vector3::DotProduct(U, position);
	result.matrix[3].Z = Vector3::DotProduct(F, position);

	return result;
}

/*
*	Calculates an ortographic projection matrix.
*/
Matrix4 Matrix4::Ortographic(const float left, const float right, const float bottom, const float top, const float nearPlane, const float farPlane) CATALYST_NOEXCEPT
{
	Matrix4 result;

	result.matrix[0].X = 2.0f / (right - left);
	result.matrix[1].Y = 2.0f / (top - bottom);
	result.matrix[3].X = -(right + left) / (right - left);
	result.matrix[3].Y = -(top + bottom) / (top - bottom);

	result.matrix[2].Z = -2.0f / (farPlane - nearPlane);
	result.matrix[3].Z = -(farPlane + nearPlane) / (farPlane - nearPlane);

	return result;
}

/*
*	Calculates a projection matrix.
*/
Matrix4 Matrix4::Perspective(const float fov, const float aspectRatio, const float nearPlane, const float farPlane) CATALYST_NOEXCEPT
{
	const float halfFovTangent = GameMath::TangentRadians(fov * 0.5f);

	Matrix4 result{ 0.0f };

	result.matrix[0].X = 1.0f / (aspectRatio * halfFovTangent);
	result.matrix[1].Y = -1.0f / (halfFovTangent);
	result.matrix[2].Z = farPlane / (nearPlane - farPlane);
	result.matrix[2].W = -1.0f;
	result.matrix[3].Z = -(farPlane * nearPlane) / (farPlane - nearPlane);

	return result;
}

/*
*	Default constructor.
*/
Matrix4::Matrix4() CATALYST_NOEXCEPT
	:
	matrix{ { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }
{

}

/*
*	Copy constructor.
*/
Matrix4::Matrix4(const Matrix4 &otherMatrix) CATALYST_NOEXCEPT
	:
	matrix{ otherMatrix.matrix[0], otherMatrix.matrix[1], otherMatrix.matrix[2], otherMatrix.matrix[3] }
{

}

/*
*	Constructor taking a scalar.
*/
Matrix4::Matrix4(const float scalar) CATALYST_NOEXCEPT
	:
	matrix{ { scalar, 0.0f, 0.0f, 0.0f }, { 0.0f, scalar, 0.0f, 0.0f }, { 0.0f, 0.0f, scalar, 0.0f }, { 0.0f, 0.0f, 0.0f, scalar } }
{

}

/*
*	Constructor taking a Matrix3.
*/
Matrix4::Matrix4(const Matrix3 &otherMatrix) CATALYST_NOEXCEPT
	:
	matrix{ { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } }
{

}

/*
*	Constructor taking the four vectors as arguments.
*/
Matrix4::Matrix4(const Vector4 &vector1, const Vector4 &vector2, const Vector4 &vector3, const Vector4 &vector4) CATALYST_NOEXCEPT
	:
	matrix{ vector1, vector2, vector3, vector4 }
{

}

/*
*	Constructor taking in position, rotation and scale as arguments.
*/
Matrix4::Matrix4(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) CATALYST_NOEXCEPT
	:
	matrix{ { scale.X, 0.0f, 0.0f, 0.0f },{ 0.0f, scale.Y, 0.0f, 0.0f },{ 0.0f, 0.0f, scale.Z, 0.0f },{ position.X, position.Y, position.Z, 1.0f } }
{
	//Create a rotation matrix for the X axis.
	if (rotation.X != 0.0f)
	{
		const float xRadians = GameMath::DegreesToRadians(rotation.X);
		const float xSine = GameMath::SineRadians(xRadians);
		const float xCosine = GameMath::CosineRadians(xRadians);

		const Matrix4 xRotationMatrix{ Vector4(1.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, xCosine, xSine, 0.0f), Vector4(0.0f, -xSine, xCosine, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f) };

		*this = *this * xRotationMatrix;
	}

	//Create a rotation matrix for the Y axis.
	if (rotation.Y != 0.0f)
	{
		const float yRadians = GameMath::DegreesToRadians(rotation.Y);
		const float ySine = GameMath::SineRadians(yRadians);
		const float yCosine = GameMath::CosineRadians(yRadians);

		const Matrix4 yRotationMatrix{ Vector4(yCosine, 0.0f, -ySine, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f), Vector4(ySine, 0.0f, yCosine, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f) };

		*this = *this * yRotationMatrix;
	}

	//Create a rotation matrix for the Z axis.
	if (rotation.Z != 0.0f)
	{
		const float zRadians = GameMath::DegreesToRadians(rotation.Z);
		const float zSine = GameMath::SineRadians(zRadians);
		const float zCosine = GameMath::CosineRadians(zRadians);

		const Matrix4 zRotationMatrix{ Vector4(zCosine, zSine, 0.0f, 0.0f), Vector4(-zSine, zCosine, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f) };

		*this = *this * zRotationMatrix;
	}
}

/*
*	Returns the translation.
*/
Vector3 Matrix4::GetTranslation() const CATALYST_NOEXCEPT
{
	return Vector3(matrix[3].X, matrix[3].Y, matrix[3].Z);
}

/*
*	Returns the scale.
*/
Vector3 Matrix4::GetScale() const CATALYST_NOEXCEPT
{
	return Vector3(matrix[0].X, matrix[1].Y, matrix[2].Z);
}

/*
*	Sets the translation.
*/
void Matrix4::SetTranslation(const Vector3 &newTranslation) CATALYST_NOEXCEPT
{
	matrix[3].X = newTranslation.X;
	matrix[3].Y = newTranslation.Y;
	matrix[3].Z = newTranslation.Z;
}

/*
*	Sets the scale.
*/
void Matrix4::SetScale(const Vector3 & newScale) CATALYST_NOEXCEPT
{
	matrix[0].X = newScale.X;
	matrix[1].Y = newScale.Y;
	matrix[2].Z = newScale.Z;
}

/*
*	Returns the determinant of this matrix.
*/
float Matrix4::GetDeterminant() CATALYST_NOEXCEPT
{
	float X1 = matrix[1].Y * ((matrix[2].Z * matrix[3].W) - (matrix[3].Z * matrix[2].W));
	float X2 = matrix[1].Z * -((matrix[2].Y * matrix[3].W) - (matrix[3].Y * matrix[2].W));
	float X3 = matrix[1].W * ((matrix[2].Y * matrix[3].Z) - (matrix[3].Y * matrix[2].Z));

	float X = matrix[0].X * (X1 + X2 + X3);

	float Y1 = matrix[1].X * ((matrix[2].Z * matrix[3].W) - (matrix[3].Z * matrix[2].W));
	float Y2 = matrix[1].Z * -((matrix[2].X * matrix[3].W) - (matrix[3].X * matrix[2].W));
	float Y3 = matrix[1].W * ((matrix[2].X * matrix[3].Z) - (matrix[3].X * matrix[2].Z));

	float Y = matrix[0].Y * -(Y1 + Y2 + Y3);

	float Z1 = matrix[1].X * ((matrix[2].Y * matrix[3].W) - (matrix[3].Y * matrix[2].W));
	float Z2 = matrix[1].Y * -((matrix[2].X * matrix[3].W) - (matrix[3].X * matrix[2].W));
	float Z3 = matrix[1].W * ((matrix[2].X * matrix[3].Y) - (matrix[3].X * matrix[2].Y));

	float Z = matrix[0].Z * (Z1 + Z2 + Z3);

	float W1 = matrix[1].X * ((matrix[2].Y * matrix[3].Z) - (matrix[3].Y * matrix[2].Z));
	float W2 = matrix[1].Y * -((matrix[2].X * matrix[3].Z) - (matrix[3].X * matrix[2].Z));
	float W3 = matrix[1].Z * ((matrix[2].X * matrix[3].Y) - (matrix[3].X * matrix[2].Y));

	float W = matrix[0].W * -(W1 + W2 + W3);

	return X + Y + Z + W;
}

/*
*	Inverses the matrix.
*/
void Matrix4::Inverse() CATALYST_NOEXCEPT
{
	float Coef00 = matrix[2].Z * matrix[3].W - matrix[3].Z * matrix[2].W;
	float Coef02 = matrix[1].Z * matrix[3].W - matrix[3].Z * matrix[1].W;
	float Coef03 = matrix[1].Z * matrix[2].W - matrix[2].Z * matrix[1].W;

	float Coef04 = matrix[2].Y * matrix[3].W - matrix[3].Y * matrix[2].W;
	float Coef06 = matrix[1].Y * matrix[3].W - matrix[3].Y * matrix[1].W;
	float Coef07 = matrix[1].Y * matrix[2].W - matrix[2].Y * matrix[1].W;

	float Coef08 = matrix[2].Y * matrix[3].Z - matrix[3].Y * matrix[2].Z;
	float Coef10 = matrix[1].Y * matrix[3].Z - matrix[3].Y * matrix[1].Z;
	float Coef11 = matrix[1].Y * matrix[2].Z - matrix[2].Y * matrix[1].Z;

	float Coef12 = matrix[2].X * matrix[3].W - matrix[3].X * matrix[2].W;
	float Coef14 = matrix[1].X * matrix[3].W - matrix[3].X * matrix[1].W;
	float Coef15 = matrix[1].X * matrix[2].W - matrix[2].X * matrix[1].W;

	float Coef16 = matrix[2].X * matrix[3].Z - matrix[3].X * matrix[2].Z;
	float Coef18 = matrix[1].X * matrix[3].Z - matrix[3].X * matrix[1].Z;
	float Coef19 = matrix[1].X * matrix[2].Z - matrix[2].X * matrix[1].Z;

	float Coef20 = matrix[2].X * matrix[3].Y - matrix[3].X * matrix[2].Y;
	float Coef22 = matrix[1].X * matrix[3].Y - matrix[3].X * matrix[1].Y;
	float Coef23 = matrix[1].X * matrix[2].Y - matrix[2].X * matrix[1].Y;

	Vector4 Fac0(Coef00, Coef00, Coef02, Coef03);
	Vector4 Fac1(Coef04, Coef04, Coef06, Coef07);
	Vector4 Fac2(Coef08, Coef08, Coef10, Coef11);
	Vector4 Fac3(Coef12, Coef12, Coef14, Coef15);
	Vector4 Fac4(Coef16, Coef16, Coef18, Coef19);
	Vector4 Fac5(Coef20, Coef20, Coef22, Coef23);

	Vector4 Vec0(matrix[1].X, matrix[0].X, matrix[0].X, matrix[0].X);
	Vector4 Vec1(matrix[1].Y, matrix[0].Y, matrix[0].Y, matrix[0].Y);
	Vector4 Vec2(matrix[1].Z, matrix[0].Z, matrix[0].Z, matrix[0].Z);
	Vector4 Vec3(matrix[1].W, matrix[0].W, matrix[0].W, matrix[0].W);

	Vector4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
	Vector4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
	Vector4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
	Vector4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

	Vector4 SignA(+1, -1, +1, -1);
	Vector4 SignB(-1, +1, -1, +1);
	Matrix4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

	Vector4 Row0(Inverse.matrix[0].X, Inverse.matrix[1].X, Inverse.matrix[2].X, Inverse.matrix[3].X);

	Vector4 Dot0(matrix[0] * Row0);
	float Dot1 = (Dot0.X + Dot0.Y) + (Dot0.Z + Dot0.W);

	float OneOverDeterminant = 1.0f / Dot1;

	Inverse *= OneOverDeterminant;

	matrix[0] = Inverse.matrix[0];
	matrix[1] = Inverse.matrix[1];
	matrix[2] = Inverse.matrix[2];
	matrix[3] = Inverse.matrix[3];
}

/*
*	Transposes the matrix.
*/
void Matrix4::Transpose() CATALYST_NOEXCEPT
{
	Vector4 transposedMatrix[4];

	transposedMatrix[0].X = matrix[0].X;
	transposedMatrix[1].X = matrix[0].Y;
	transposedMatrix[2].X = matrix[0].Z;
	transposedMatrix[3].X = matrix[0].W;

	transposedMatrix[0].Y = matrix[1].X;
	transposedMatrix[1].Y = matrix[1].Y;
	transposedMatrix[2].Y = matrix[1].Z;
	transposedMatrix[3].Y = matrix[1].W;

	transposedMatrix[0].Z = matrix[2].X;
	transposedMatrix[1].Z = matrix[2].Y;
	transposedMatrix[2].Z = matrix[2].Z;
	transposedMatrix[3].Z = matrix[2].W;

	transposedMatrix[0].W = matrix[3].X;
	transposedMatrix[1].W = matrix[3].Y;
	transposedMatrix[2].W = matrix[3].Z;
	transposedMatrix[3].W = matrix[3].W;

	matrix[0] = transposedMatrix[0];
	matrix[1] = transposedMatrix[1];
	matrix[2] = transposedMatrix[2];
	matrix[3] = transposedMatrix[3];
}

/*
*	Multiplication overload.
*/
Matrix4 Matrix4::operator*(const Matrix4 &otherMatrix) const CATALYST_NOEXCEPT
{
	Matrix4 multipliedMatrix{ otherMatrix };

	multipliedMatrix.matrix[0].X = (matrix[0].X * otherMatrix.matrix[0].X) + (matrix[1].X * otherMatrix.matrix[0].Y) + (matrix[2].X * otherMatrix.matrix[0].Z) + (matrix[3].X * otherMatrix.matrix[0].W);
	multipliedMatrix.matrix[1].X = (matrix[0].X * otherMatrix.matrix[1].X) + (matrix[1].X * otherMatrix.matrix[1].Y) + (matrix[2].X * otherMatrix.matrix[1].Z) + (matrix[3].X * otherMatrix.matrix[1].W);
	multipliedMatrix.matrix[2].X = (matrix[0].X * otherMatrix.matrix[2].X) + (matrix[1].X * otherMatrix.matrix[2].Y) + (matrix[2].X * otherMatrix.matrix[2].Z) + (matrix[3].X * otherMatrix.matrix[2].W);
	multipliedMatrix.matrix[3].X = (matrix[0].X * otherMatrix.matrix[3].X) + (matrix[1].X * otherMatrix.matrix[3].Y) + (matrix[2].X * otherMatrix.matrix[3].Z) + (matrix[3].X * otherMatrix.matrix[3].W);

	multipliedMatrix.matrix[0].Y = (matrix[0].Y * otherMatrix.matrix[0].X) + (matrix[1].Y * otherMatrix.matrix[0].Y) + (matrix[2].Y * otherMatrix.matrix[0].Z) + (matrix[3].Y * otherMatrix.matrix[0].W);
	multipliedMatrix.matrix[1].Y = (matrix[0].Y * otherMatrix.matrix[1].X) + (matrix[1].Y * otherMatrix.matrix[1].Y) + (matrix[2].Y * otherMatrix.matrix[1].Z) + (matrix[3].Y * otherMatrix.matrix[1].W);
	multipliedMatrix.matrix[2].Y = (matrix[0].Y * otherMatrix.matrix[2].X) + (matrix[1].Y * otherMatrix.matrix[2].Y) + (matrix[2].Y * otherMatrix.matrix[2].Z) + (matrix[3].Y * otherMatrix.matrix[2].W);
	multipliedMatrix.matrix[3].Y = (matrix[0].Y * otherMatrix.matrix[3].X) + (matrix[1].Y * otherMatrix.matrix[3].Y) + (matrix[2].Y * otherMatrix.matrix[3].Z) + (matrix[3].Y * otherMatrix.matrix[3].W);

	multipliedMatrix.matrix[0].Z = (matrix[0].Z * otherMatrix.matrix[0].X) + (matrix[1].Z * otherMatrix.matrix[0].Y) + (matrix[2].Z * otherMatrix.matrix[0].Z) + (matrix[3].Z * otherMatrix.matrix[0].W);
	multipliedMatrix.matrix[1].Z = (matrix[0].Z * otherMatrix.matrix[1].X) + (matrix[1].Z * otherMatrix.matrix[1].Y) + (matrix[2].Z * otherMatrix.matrix[1].Z) + (matrix[3].Z * otherMatrix.matrix[1].W);
	multipliedMatrix.matrix[2].Z = (matrix[0].Z * otherMatrix.matrix[2].X) + (matrix[1].Z * otherMatrix.matrix[2].Y) + (matrix[2].Z * otherMatrix.matrix[2].Z) + (matrix[3].Z * otherMatrix.matrix[2].W);
	multipliedMatrix.matrix[3].Z = (matrix[0].Z * otherMatrix.matrix[3].X) + (matrix[1].Z * otherMatrix.matrix[3].Y) + (matrix[2].Z * otherMatrix.matrix[3].Z) + (matrix[3].Z * otherMatrix.matrix[3].W);

	multipliedMatrix.matrix[0].W = (matrix[0].W * otherMatrix.matrix[0].X) + (matrix[1].W * otherMatrix.matrix[0].Y) + (matrix[2].W * otherMatrix.matrix[0].Z) + (matrix[3].W * otherMatrix.matrix[0].W);
	multipliedMatrix.matrix[1].W = (matrix[0].W * otherMatrix.matrix[1].X) + (matrix[1].W * otherMatrix.matrix[1].Y) + (matrix[2].W * otherMatrix.matrix[1].Z) + (matrix[3].W * otherMatrix.matrix[1].W);
	multipliedMatrix.matrix[2].W = (matrix[0].W * otherMatrix.matrix[2].X) + (matrix[1].W * otherMatrix.matrix[2].Y) + (matrix[2].W * otherMatrix.matrix[2].Z) + (matrix[3].W * otherMatrix.matrix[2].W);
	multipliedMatrix.matrix[3].W = (matrix[0].W * otherMatrix.matrix[3].X) + (matrix[1].W * otherMatrix.matrix[3].Y) + (matrix[2].W * otherMatrix.matrix[3].Z) + (matrix[3].W * otherMatrix.matrix[3].W);

	return multipliedMatrix;
}

/*
*	Matrix4 by scalar multiplication assignment overload.
*/
void Matrix4::operator*=(const float scalar) CATALYST_NOEXCEPT
{
	matrix[0].X *= scalar;
	matrix[0].Y *= scalar;
	matrix[0].Z *= scalar;
	matrix[0].W *= scalar;

	matrix[1].X *= scalar;
	matrix[1].Y *= scalar;
	matrix[1].Z *= scalar;
	matrix[1].W *= scalar;

	matrix[2].X *= scalar;
	matrix[2].Y *= scalar;
	matrix[2].Z *= scalar;
	matrix[2].W *= scalar;

	matrix[3].X *= scalar;
	matrix[3].Y *= scalar;
	matrix[3].Z *= scalar;
	matrix[3].W *= scalar;
}

/*
*	Matrix/Vector multiplication overload.
*/
Vector4 Matrix4::operator*(const Vector4 &vector) const CATALYST_NOEXCEPT
{
	Vector4 multipliedVector{ };

	multipliedVector.X = (matrix[0].X * vector.X) + (matrix[1].X * vector.Y) + (matrix[2].X * vector.Z) + (matrix[3].X * vector.W);
	multipliedVector.Y = (matrix[0].Y * vector.X) + (matrix[1].Y * vector.Y) + (matrix[2].Y * vector.Z) + (matrix[3].Y * vector.W);
	multipliedVector.Z = (matrix[0].Z * vector.X) + (matrix[1].Z * vector.Y) + (matrix[2].Z * vector.Z) + (matrix[3].Z * vector.W);
	multipliedVector.W = (matrix[0].W * vector.X) + (matrix[1].W * vector.Y) + (matrix[2].W * vector.Z) + (matrix[3].W * vector.W);

	return multipliedVector;
}