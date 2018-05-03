#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Catalyst Math constants.
namespace CatalystMathConstants
{
	constexpr float SINE_PRECISION{ 0.1f };
	constexpr float DEGREES_TO_RADIANS{ 0.017'453f };
	constexpr float PI{ 3.141'592f };
	constexpr float RADIANS_TO_DEGREES{ 57.295'779f };
}

class CatalystMath final
{

public:

	/*
	*	Returns the absolute value of the floating point number passed in.
	*/
	constexpr static float Absolute(const float value) NOEXCEPT
	{
		return value >= 0.0f ? value : value * -1.0f;
	}

	/*
	*	Given a number, denoted in degrees, returns the arcsine of the number.
	*/
	static float ArcsineDegrees(const float number) NOEXCEPT
	{
		return ArcsineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number, denoted in radians, returns the arcsine of the number.
	*/
	static float ArcsineRadians(const float number) NOEXCEPT
	{
		return asinf(number);
	}

	/*
	*	Given a number, denoted in degrees, returns the arctangent of the number.
	*/
	static float ArctangentDegrees(const float number) NOEXCEPT
	{
		return ArctangentRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number, denoted in radians, returns the arctangent of the number.
	*/
	static float ArctangentRadians(const float number) NOEXCEPT
	{
		return atanf(number);
	}

	/*
	*	Given two numbers, denoted in degrees, returns the arctangent of the number.
	*/
	static float ArctangentDegrees(const float number1, const float number2) NOEXCEPT
	{
		return ArctangentRadians(DegreesToRadians(number1), DegreesToRadians(number2));
	}

	/*
	*	Given two numbers, denoted in radians, returns the arctangent of the number.
	*/
	static float ArctangentRadians(const float number1, const float number2) NOEXCEPT
	{
		return atan2f(number1, number2);
	}

	/*
	*	Rounds a number up to the nearest integer.
	*/
	template <class IntegerType>
	static constexpr IntegerType Ceiling(const float number) NOEXCEPT
	{
		return static_cast<IntegerType>(number + 1.0f);
	}

	/*
	*	Clamps a value between a lower and an upper limit and returns the clamped value.
	*/
	template <class NumberType>
	constexpr static NumberType Clamp(const NumberType value, const NumberType lowerLimit, const NumberType upperLimit) NOEXCEPT
	{
		return value < lowerLimit ? lowerLimit : value > upperLimit ? upperLimit : value;
	}

	/*
	*	Given a degrees value, returns the cosine of the angle.
	*/
	static float CosineDegrees(const float number) NOEXCEPT
	{
		return CosineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a radians value, returns the cosine of the angle.
	*/
	static float CosineRadians(const float number) NOEXCEPT
	{
		return cos(number);
	}

	/*
	*	Given a number in degrees, return it in radians.
	*/
	constexpr static float DegreesToRadians(const float number) NOEXCEPT
	{
		return number * CatalystMathConstants::DEGREES_TO_RADIANS;
	}

	/*
	*	Given an integral number, return the factorial of that number.
	*/
	static constexpr uint64 Factorial(const uint8 number) NOEXCEPT
	{
		return number != 0 ? number * Factorial(number - 1) : 1;
	}

	/*
	*	Rounds a number down to the nearest integer.
	*/
	template <class IntegerType>
	static constexpr IntegerType Floor(const float number) NOEXCEPT
	{
		return static_cast<IntegerType>(number);
	}

	/*
	*	Given a float, return the fractional part of that float.
	*/
	static constexpr float Fractional(const float number) NOEXCEPT
	{
		return number - static_cast<float>(static_cast<int32>(number));
	}

	/*
	*	Returns whether or not an integer is even or not.
	*/
	template <class IntegerType>
	constexpr static IntegerType IsEven(const IntegerType number) NOEXCEPT
	{
		return Modulo(number, 2) == 0;
	}

	/*
	*	Returns whether or not an integer is odd or not.
	*/
	template <class IntegerType>
	constexpr static IntegerType IsOdd(const IntegerType number) NOEXCEPT
	{
		return Modulo(number, 2) != 0;
	}

	/*
	*	Linearly interpolate between two float values.
	*/
	constexpr static float LinearlyInterpolate(const float valueA, const float valueB, const float alpha) NOEXCEPT
	{
		return (valueA * (1.0f - alpha)) + (valueB * alpha);
	}

	/*
	*	Returns the maximum of two numbers.
	*/
	template <class NumberType>
	constexpr static NumberType Maximum(const NumberType firstNumber, const NumberType secondNumber) NOEXCEPT
	{
		return LIKELY(firstNumber > secondNumber) ? firstNumber : secondNumber;
	}

	/*
	*	Returns the minimum of two numbers.
	*/
	template <class NumberType>
	constexpr static NumberType Minimum(const NumberType firstNumber, const NumberType secondNumber) NOEXCEPT
	{
		return LIKELY(firstNumber < secondNumber) ? firstNumber : secondNumber;
	}

	/*
	*	Returns the modulo of two integers.
	*/
	template <class FirstIntegerType, class SecondIntegerType>
	constexpr static FirstIntegerType Modulo(const FirstIntegerType input, const SecondIntegerType ceiling) NOEXCEPT
	{
		return UNLIKELY(input >= ceiling) ? input % ceiling : input;
	}

	/*
	*	Given a number and an exponent, returns the power of the exponent.
	*/
	constexpr static float PowerOf(float number, const uint8 exponent) NOEXCEPT
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
	constexpr static float RadiansToDegrees(const float number) NOEXCEPT
	{
		return number * CatalystMathConstants::RADIANS_TO_DEGREES;
	}

	/*
	*	Given a range, returns a floating point value in that range.
	*/
	static float RandomFloatInRange(const float minimum, const float maximum) NOEXCEPT
	{
		static thread_local std::random_device randomDevice;
		static thread_local std::mt19937 randomEngine{ 0 };

		std::uniform_real_distribution<float> distribution{ minimum, maximum };

		return distribution(randomEngine);
	}

	/*
	*	Given a range, returns an integer value in that range.
	*/
	template <class IntegerType>
	static IntegerType RandomIntegerInRange(const IntegerType minimum, const IntegerType maximum) NOEXCEPT
	{
		static thread_local std::random_device randomDevice;
		static thread_local std::mt19937 randomEngine{ randomDevice() };

		std::uniform_int_distribution<IntegerType> distribution{ minimum, maximum };

		return distribution(randomEngine);
	}

	/*
	*	Given a number between 0.0f and 1.0f, representing a chance in percent, return whether or not it succeeded.
	*/
	static bool RandomChance(const float chance)
	{
		return chance > RandomFloatInRange(0.0f, 1.0f);
	}

	/*
	*	Rounds a float to the nearest integral value, with halfway cases rounded away from zero.
	*/
	template <class IntegerType>
	constexpr static IntegerType Round(const float value) NOEXCEPT
	{
		return static_cast<IntegerType>(value + 0.5f);
	}

	/*
	*	Given a value and an original range, scale the value to a range between 0.0f and 1.0f.
	*/
	static void ScaleFloatToNormalizedRange(float &value, const float originalRangeMinimum, const float originalRangeMaximum) NOEXCEPT
	{
		value = (value - originalRangeMinimum) / (originalRangeMaximum - originalRangeMinimum);
	}

	/*
	*	Given a number, returns the signum of that number.
	*/
	static constexpr float Signum(const float number) NOEXCEPT
	{
		return static_cast<float>((number > 0.0f) - (number < 0.0f));
	}

	/*
	*	Given a degrees value, returns the sine of the angle.
	*/
	static float SineDegrees(const float number) NOEXCEPT
	{
		return SineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a radians value, returns the sine of the angle.
	*/
	static float SineRadians(float number) NOEXCEPT
	{
		return sin(number);
	}

	/*
	*	Given a number and a number of derivaties, generate a smooth number.
	*/
	template <uint64 Derivatives>
	static constexpr float SmoothStep(const float number)
	{
		float smoothedNumber = number;

		for (uint64 i = 0; i < Derivatives; ++i)
		{
			smoothedNumber = smoothedNumber * smoothedNumber * (3.0f - 2.0f * smoothedNumber);
		}

		return smoothedNumber;
	}

	/*
	*	Given a number, returns the square root.
	*/
	constexpr static float SquareRoot(const float number) NOEXCEPT
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
	static float TangentDegrees(const float number) NOEXCEPT
	{
		return TangentRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number in radians, return the tangent.
	*/
	static float TangentRadians(const float number) NOEXCEPT
	{
		return tan(number);
	}

};