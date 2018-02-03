#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <MathematicalConstants.h>

//Preprocessor defines.
#define GAME_MATH_SINE_PRECISION 0.1f

class GameMath final
{

public:

	/*
	*	Returns the absolute value of the one passed in.
	*/
	constexpr static float Absolute(const float value) CATALYST_NOEXCEPT
	{
		return value >= 0.0f ? value : value * -1.0f;

	}

	/*
	*	Clamps a float value between a lower and an upper limit and returns the clamped value.
	*/
	constexpr static float Clamp(const float value, const float lowerLimit, const float upperLimit) CATALYST_NOEXCEPT
	{
		return value < lowerLimit ? lowerLimit : value > upperLimit ? upperLimit : value;
	}

	/*
	*	Given a degrees value, returns the cosine of the angle.
	*/
	static float CosineDegrees(const float number) CATALYST_NOEXCEPT
	{
		return CosineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a radians value, returns the cosine of the angle.
	*/
	static float CosineRadians(const float number) CATALYST_NOEXCEPT
	{
		return cos(number);
	}

	/*
	*	Given a number in degrees, return it in radians.
	*/
	constexpr static float DegreesToRadians(const float number) CATALYST_NOEXCEPT
	{
		return number * DEGREES_TO_RADIANS;
	}

	/*
	*	Given an integral number, return the factorial of that number.
	*/
	static constexpr uint64 Factorial(const uint8 number) CATALYST_NOEXCEPT
	{
		return number != 0 ? number * Factorial(number - 1) : 1;
	}

	/*
	*	Given an interpolation value in the range 0.0f to 1.0f, returns a smooth interpolation value.
	*/
	constexpr static float GetSmoothInterpolationValue(const float linearInterpolationValue) CATALYST_NOEXCEPT
	{
		return linearInterpolationValue * linearInterpolationValue * (3.0f - 2.0f * linearInterpolationValue);
	}

	/*
	*	Given an interpolation value in the range 0.0f to 1.0f, returns a smoother interpolation value.
	*/
	constexpr static float GetSmootherInterpolationValue(const float linearInterpolationValue) CATALYST_NOEXCEPT
	{
		return linearInterpolationValue * linearInterpolationValue * linearInterpolationValue * (linearInterpolationValue * (linearInterpolationValue * 6.0f - 15.0f) + 10.0f);
	}

	/*
	*	Returns whether or not an integer is even or not.
	*/
	template <class IntegerType>
	constexpr static IntegerType IsEven(const IntegerType number) CATALYST_NOEXCEPT
	{
		return Modulo(number, 2) == 0;
	}

	/*
	*	Returns whether or not an integer is odd or not.
	*/
	template <class IntegerType>
	constexpr static IntegerType IsOdd(const IntegerType number) CATALYST_NOEXCEPT
	{
		return Modulo(number, 2) != 0;
	}

	/*
	*	Linearly interpolate between two values.
	*/
	constexpr static float LinearlyInterpolate(const float valueA, const float valueB, const float alpha) CATALYST_NOEXCEPT
	{
		return (valueA * (1.0f - alpha)) + (valueB * alpha);
	}

	/*
	*	Returns the maximum of two numbers.
	*/
	template <class NumberType>
	constexpr static NumberType Maximum(const NumberType firstNumber, const NumberType secondNumber) CATALYST_NOEXCEPT
	{
		return CATALYST_LIKELY(firstNumber > secondNumber) ? firstNumber : secondNumber;
	}

	/*
	*	Returns the minimum of two numbers.
	*/
	template <class NumberType>
	constexpr static NumberType Minimum(const NumberType firstNumber, const NumberType secondNumber) CATALYST_NOEXCEPT
	{
		return CATALYST_LIKELY(firstNumber < secondNumber) ? firstNumber : secondNumber;
	}

	/*
	*	Returns the modulo of two integers.
	*/
	template <class IntegerType>
	constexpr static IntegerType Modulo(const IntegerType input, const IntegerType ceiling) CATALYST_NOEXCEPT
	{
		return CATALYST_UNLIKELY(input >= ceiling) ? input % ceiling : input;
	}

	/*
	*	Given a number and an exponent, returns the power of the exponent.
	*/
	constexpr static float PowerOf(float number, const uint8 exponent) CATALYST_NOEXCEPT
	{
		const float originalNumber = number;

		for (uint8 i = 1; i < exponent; ++i)
		{
			number *= originalNumber;
		}

		return number;
	}

	/*
	*	Given a number in radians, return it in degrees.
	*/
	constexpr static float RadiansToDegrees(const float number) CATALYST_NOEXCEPT
	{
		return number * RADIANS_TO_DEGREES;
	}

	/*
	*	Given a range, returns a floating point value in that range.
	*/
	static float RandomFloatInRange(const float minimum, const float maximum) CATALYST_NOEXCEPT
	{
		std::uniform_real_distribution<float> distribution(minimum, maximum);

		return distribution(randomEngine);
	}

	/*
	*	Rounds a float to the nearest integral value, with halfway cases rounded away from zero.
	*/
	static float RoundToFloat(const float value) CATALYST_NOEXCEPT
	{
		return round(value);
	}

	/*
	*	Rounds a float to the nearest integral value, with halfway cases rounded away from zero.
	*/
	template <class IntegerType>
	constexpr static IntegerType RoundToInteger(const float value) CATALYST_NOEXCEPT
	{
		return static_cast<IntegerType>(round(value));
	}

	/*
	*	Given a value and an original range, scale the value to a range between 0.0f and 1.0f.
	*/
	static void ScaleFloatToNormalizedRange(float &value, const float originalRangeMinimum, const float originalRangeMaximum) CATALYST_NOEXCEPT
	{
		value = (value - originalRangeMinimum) / (originalRangeMaximum - originalRangeMinimum);
	}

	/*
	*	Given a number, returns the signum of that number.
	*/
	static constexpr float Signum(const float number) CATALYST_NOEXCEPT
	{
		return static_cast<float>((number > 0.0f) - (number < 0.0f));
	}

	/*
	*	Given a degrees value, returns the sine of the angle.
	*/
	static float SineDegrees(const float number) CATALYST_NOEXCEPT
	{
		return SineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a radians value, returns the sine of the angle.
	*/
	static float SineRadians(float number) CATALYST_NOEXCEPT
	{
		return sin(number);
	}

	/*
	*	Given a number, returns the square root.
	*/
	constexpr static float SquareRoot(const float number) CATALYST_NOEXCEPT
	{
		float squareRoot = number * 0.25f;

		for (uint8 i = 0; i < 10; ++i)
		{
			squareRoot = (squareRoot + (number / squareRoot)) / 2;
		}

		return squareRoot;
	}

	/*
	*	Given a number in degrees, return the tangent.
	*/
	static float TangentDegrees(const float number) CATALYST_NOEXCEPT
	{
		return TangentRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number in radians, return the tangent.
	*/
	static float TangentRadians(const float number) CATALYST_NOEXCEPT
	{
		return tan(number);
	}

private:

	//The random device used to generate random numbers.
	static std::random_device randomDevice;

	//The random engine used to generate random numbers.
	static std::mt19937 randomEngine;

};