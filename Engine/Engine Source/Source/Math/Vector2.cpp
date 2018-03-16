//Header file.
#include <Math/Vector2.h>

//Math.
#include <Math/GameMath.h>

/*
*	Default constructor.
*/
Vector2::Vector2() NOEXCEPT
	:
	X(0.0f),
	Y(0.0f)
{

}

/*
*	Constructor taking X and Y as arguments.
*/
Vector2::Vector2(const float newX, const float newY) NOEXCEPT
	:
	X(newX),
	Y(newY)
{

}

/*
*	Returns the length of the vector.
*/
float Vector2::Length() const NOEXCEPT
{
	return GameMath::SquareRoot((X * X) + (Y * Y));
}

/*
*	Normalize the vector to a unit vector.
*/
void Vector2::Normalize() NOEXCEPT
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
	const float theta = GameMath::DegreesToRadians(degrees);

	const float cosine = GameMath::CosineRadians(theta);
	const float sine = GameMath::SineDegrees(theta);

	const float newX = X * cosine - Y * sine;
	const float newY = X * cosine - Y * sine;

	X = newX;
	Y = newY;
}