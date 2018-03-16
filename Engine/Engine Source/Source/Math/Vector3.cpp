//Header file.
#include <Math/Vector3.h>

//Math.
#include <Math/CatalystMath.h>

/*
*	Returns the dot product of two vectors.
*/
float Vector3::DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
{
	return (firstVector.X * secondVector.X) + (firstVector.Y * secondVector.Y) + (firstVector.Z * secondVector.Z);
}

/*
*	Returns the cross product of two vectors.
*/
Vector3 Vector3::CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) NOEXCEPT
{
	return Vector3{ (firstVector.Y * secondVector.Z) - (firstVector.Z * secondVector.Y), (firstVector.Z * secondVector.X) - (firstVector.X * secondVector.Z), (firstVector.X * secondVector.Y) - (firstVector.Y * secondVector.X) };
}

/*
*	Given a vector, return it's length squared.
*/
float Vector3::LengthSquared(const Vector3 &vector) NOEXCEPT
{
	const float absoluteX = CatalystMath::Absolute(vector.X);
	const float absoluteY = CatalystMath::Absolute(vector.Y);
	const float absoluteZ = CatalystMath::Absolute(vector.Z);

	return (absoluteX * absoluteX) + (absoluteY * absoluteY) + (absoluteZ * absoluteZ);
}

/*
*	Given a vector, return it's unit vector counterpart.
*/
Vector3 Vector3::Normalize(const Vector3 &vector) NOEXCEPT
{
	Vector3 unitVector{ vector };

	unitVector.Normalize();

	return unitVector;
}

/*
*	Returns the length of the vector.
*/
float Vector3::Length() const NOEXCEPT
{
	return CatalystMath::SquareRoot((X * X) + (Y * Y) + (Z *Z));
}

/*
*	Normalize the vector to a unit vector.
*/
void Vector3::Normalize() NOEXCEPT
{
	const float length = Length();

	X /= length;
	Y /= length;
	Z /= length;
}

/*
*	Given a rotation vector, rotate this vector.
*/
void Vector3::Rotate(const Vector3& rotationVector) NOEXCEPT
{
	//Rotate the roll.
	float tempY = Y * CatalystMath::CosineDegrees(rotationVector.X) - Z * CatalystMath::SineDegrees(rotationVector.X);
	Z = Y * CatalystMath::SineDegrees(rotationVector.X) + Z * CatalystMath::CosineDegrees(rotationVector.X);
	Y = tempY;
	
	//Rotate the pitch
	float tempX1 = X * CatalystMath::CosineDegrees(rotationVector.Y) + Z * CatalystMath::SineDegrees(rotationVector.Y);
	Z = -X * CatalystMath::SineDegrees(rotationVector.Y) + Z * CatalystMath::CosineDegrees(rotationVector.Y);
	X = tempX1;

	//Rotate the yaw.
	float tempX2 = X * CatalystMath::CosineDegrees(rotationVector.Z) - Y * CatalystMath::SineDegrees(rotationVector.Z);
	Y = X * CatalystMath::SineDegrees(rotationVector.Z) + Y * CatalystMath::CosineDegrees(rotationVector.Z);
	X = tempX2;
}

/*
*	Given a rotation vector, rotate this vector.
*/
void Vector3::NewRotate(const Vector3& rotationVector) NOEXCEPT
{
	const float xRadians = CatalystMath::DegreesToRadians(rotationVector.X);
	const float yRadians = CatalystMath::DegreesToRadians(rotationVector.Y);
	const float zRadians = CatalystMath::DegreesToRadians(rotationVector.Z);

	//Rotate the roll.
	float tempY = Y * CatalystMath::CosineRadians(xRadians) - Z * CatalystMath::SineRadians(xRadians);
	Z = Y * CatalystMath::SineRadians(xRadians) + Z * CatalystMath::CosineRadians(xRadians);
	Y = tempY;

	//Rotate the pitch
	float tempX1 = X * CatalystMath::CosineRadians(yRadians) + Z * CatalystMath::SineRadians(yRadians);
	Z = -X * CatalystMath::SineRadians(yRadians) + Z * CatalystMath::CosineRadians(yRadians);
	X = tempX1;

	//Rotate the yaw.
	float tempX2 = X * CatalystMath::CosineRadians(zRadians) - Y * CatalystMath::SineRadians(zRadians);
	Y = X * CatalystMath::SineRadians(zRadians) + Y * CatalystMath::CosineRadians(zRadians);
	X = tempX2;
}

/*
*	Returns a rotate copy of this vector.
*/
Vector3 Vector3::Rotated(const Vector3& rotationVector) const NOEXCEPT
{
	Vector3 rotatedVector{ *this };
	rotatedVector.Rotate(rotationVector);

	return rotatedVector;
}