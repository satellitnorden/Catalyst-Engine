//Header file.
#include <Math/Core/BaseMath.h>

//STL.
#define _USE_MATH_DEFINES
#include <cmath>

/*
*	Given a number, returns the arcsine of the number.
*/
NO_DISCARD float32 BaseMath::ArcCosine(const float32 number) NOEXCEPT
{
	return acosf(number);
}

/*
*	Given a number, returns the arcsine of the number.
*/
NO_DISCARD float32 BaseMath::ArcSine(const float32 number) NOEXCEPT
{
	return asinf(number);
}

/*
*	Given a number, returns the arctangent of the number.
*/
NO_DISCARD float32 BaseMath::ArcTangent(const float32 number) NOEXCEPT
{
	return atanf(number);
}

/*
*	Given two numbers, returns the arctangent of the number.
*/
NO_DISCARD float32 BaseMath::ArcTangent(const float32 Y, const float32 X) NOEXCEPT
{
	return atan2f(Y, X);
}

/*
*	Returns the cube root of the given number.
*/
NO_DISCARD float32 BaseMath::CubeRoot(const float32 number) NOEXCEPT
{
	return std::cbrtf(number);
}

/*
*	Returns the error function of the given number.
*/
NO_DISCARD float32 BaseMath::ErrorFunction(const float32 number) NOEXCEPT
{
	return std::erf(number);
}

/*
*	Returns e (Euler's number) raised to the given power.
*/
NO_DISCARD float32 BaseMath::Exponential(const float32 power) NOEXCEPT
{
	return std::exp(power);
}

/*
*	Returns the natural (base e) logarithm of the given number.
*/
NO_DISCARD float32 BaseMath::Logarithm(const float32 number) NOEXCEPT
{
	return std::logf(number);
}

/*
*	Given a number, returns the hyperbolic tangent of the number.
*/
NO_DISCARD float32 BaseMath::HyperbolicTangent(const float32 number) NOEXCEPT
{
	return std::tanh(number);
}

/*
*	Given a number, returns the square root.
*/
NO_DISCARD float32 BaseMath::SquareRoot(float32 number) NOEXCEPT
{
	if (number == 0.0f)
	{
		return 0.0f;
	}

	int32 exponent{ 0 };

	number = frexp(number, &exponent);

	if (exponent & 1)
	{
		--exponent;
		number *= 2.0f;
	}

	float32 approximation{ (1.0f + number) * 0.5f };
	float32 target{ 0.0f };

	while (approximation != target)
	{
		target = approximation;
		approximation = (approximation + number / approximation) * 0.5f;
	}

	return ldexp(approximation, exponent / 2);
}