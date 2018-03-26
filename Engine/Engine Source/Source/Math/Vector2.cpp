//Header file.
#include <Math/Vector2.h>

//Math.
#include <Math/CatalystMath.h>

/*
*	Returns the length of the vector.
*/
constexpr float Vector2::Length() const NOEXCEPT
{
	return CatalystMath::SquareRoot((X * X) + (Y * Y));
}

/*
*	Normalize the vector to a unit vector.
*/
constexpr void Vector2::Normalize() NOEXCEPT
{
	const float length = Length();

	X /= length;
	Y /= length;
}

/*
*	Given an amount of degrees, rotate the vector.
*/
void Vector2::Rotate(const float degrees) NOEXCEPT
{
	const float theta = CatalystMath::DegreesToRadians(degrees);

	const float cosine = CatalystMath::CosineRadians(theta);
	const float sine = CatalystMath::SineDegrees(theta);

	const float newX = X * cosine - Y * sine;
	const float newY = X * cosine - Y * sine;

	X = newX;
	Y = newY;
}