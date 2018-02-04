//Header file.
#include <Vector3.h>

//Math.
#include <GameMath.h>

/*
*	Returns the dot product of two vectors.
*/
float Vector3::DotProduct(const Vector3 &firstVector, const Vector3 &secondVector) CATALYST_NOEXCEPT
{
	return (firstVector.X * secondVector.X) + (firstVector.Y * secondVector.Y) + (firstVector.Z * secondVector.Z);
}

/*
*	Returns the cross product of two vectors.
*/
Vector3 Vector3::CrossProduct(const Vector3 &firstVector, const Vector3 &secondVector) CATALYST_NOEXCEPT
{
	return Vector3{ (firstVector.Y * secondVector.Z) - (firstVector.Z * secondVector.Y), (firstVector.Z * secondVector.X) - (firstVector.X * secondVector.Z), (firstVector.X * secondVector.Y) - (firstVector.Y * secondVector.X) };
}

/*
*	Given a vector, return it's length squared.
*/
float Vector3::LengthSquared(const Vector3 &vector) CATALYST_NOEXCEPT
{
	const float absoluteX = GameMath::Absolute(vector.X);
	const float absoluteY = GameMath::Absolute(vector.Y);
	const float absoluteZ = GameMath::Absolute(vector.Z);

	return (absoluteX * absoluteX) + (absoluteY * absoluteY) + (absoluteZ * absoluteZ);
}

/*
*	Given a vector, return it's unit vector counterpart.
*/
Vector3 Vector3::Normalize(const Vector3 &vector) CATALYST_NOEXCEPT
{
	Vector3 unitVector{ vector };

	unitVector.Normalize();

	return unitVector;
}

/*
*	Returns the length of the vector.
*/
float Vector3::Length() const CATALYST_NOEXCEPT
{
	return GameMath::SquareRoot((X * X) + (Y * Y) + (Z *Z));
}

/*
*	Normalize the vector to a unit vector.
*/
void Vector3::Normalize() CATALYST_NOEXCEPT
{
	const float length = Length();

	X /= length;
	Y /= length;
	Z /= length;
}

/*
*	Given a rotation vector, rotate this vector.
*/
void Vector3::Rotate(const Vector3& rotationVector) CATALYST_NOEXCEPT
{
	//Rotate the roll.
	float tempY = Y * GameMath::CosineDegrees(rotationVector.X) - Z * GameMath::SineDegrees(rotationVector.X);
	Z = Y * GameMath::SineDegrees(rotationVector.X) + Z * GameMath::CosineDegrees(rotationVector.X);
	Y = tempY;
	
	//Rotate the pitch
	float tempX1 = X * GameMath::CosineDegrees(rotationVector.Y) + Z * GameMath::SineDegrees(rotationVector.Y);
	Z = -X * GameMath::SineDegrees(rotationVector.Y) + Z * GameMath::CosineDegrees(rotationVector.Y);
	X = tempX1;

	//Rotate the yaw.
	float tempX2 = X * GameMath::CosineDegrees(rotationVector.Z) - Y * GameMath::SineDegrees(rotationVector.Z);
	Y = X * GameMath::SineDegrees(rotationVector.Z) + Y * GameMath::CosineDegrees(rotationVector.Z);
	X = tempX2;
}

/*
*	Given a rotation vector, rotate this vector.
*/
void Vector3::NewRotate(const Vector3& rotationVector) CATALYST_NOEXCEPT
{
	const float xRadians = GameMath::DegreesToRadians(rotationVector.X);
	const float yRadians = GameMath::DegreesToRadians(rotationVector.Y);
	const float zRadians = GameMath::DegreesToRadians(rotationVector.Z);

	//Rotate the roll.
	float tempY = Y * GameMath::CosineRadians(xRadians) - Z * GameMath::SineRadians(xRadians);
	Z = Y * GameMath::SineRadians(xRadians) + Z * GameMath::CosineRadians(xRadians);
	Y = tempY;

	//Rotate the pitch
	float tempX1 = X * GameMath::CosineRadians(yRadians) + Z * GameMath::SineRadians(yRadians);
	Z = -X * GameMath::SineRadians(yRadians) + Z * GameMath::CosineRadians(yRadians);
	X = tempX1;

	//Rotate the yaw.
	float tempX2 = X * GameMath::CosineRadians(zRadians) - Y * GameMath::SineRadians(zRadians);
	Y = X * GameMath::SineRadians(zRadians) + Y * GameMath::CosineRadians(zRadians);
	X = tempX2;
}

/*
*	Returns a rotate copy of this vector.
*/
Vector3 Vector3::Rotated(const Vector3& rotationVector) const CATALYST_NOEXCEPT
{
	Vector3 rotatedVector{ *this };
	rotatedVector.Rotate(rotationVector);

	return rotatedVector;
}