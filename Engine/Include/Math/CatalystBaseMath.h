#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Catalyst base math constants.
namespace CatalystBaseMathConstants
{
	constexpr float DEGREES_TO_RADIANS{ 0.017'453f };
	constexpr float PI{ 3.141'592f };
	constexpr float DOUBLE_PI{ PI * 2.0f };
	constexpr float HALF_PI{ PI * 0.5f };
	constexpr float INVERSE_PI{ 1.0f / PI };
	constexpr float RADIANS_TO_DEGREES{ 57.295'779f };
}

class CatalystBaseMath final
{

public:

	/*
	*	Returns the absolute value of the floating point number passed in.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Absolute(const float value) NOEXCEPT
	{
		return value >= 0.0f ? value : value * -1.0f;
	}

	/*
	*	Given a number, denoted in degrees, returns the arc cosine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArcCosineDegrees(const float number) NOEXCEPT
	{
		return ArcCosineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number, denoted in radians, returns the arcsine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArcCosineRadians(const float number) NOEXCEPT
	{
		return acosf(number);
	}

	/*
	*	Given a number, denoted in degrees, returns the arcsine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArcsineDegrees(const float number) NOEXCEPT
	{
		return ArcsineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number, denoted in radians, returns the arcsine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArcsineRadians(const float number) NOEXCEPT
	{
		return asinf(number);
	}

	/*
	*	Given a number, denoted in degrees, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArctangentDegrees(const float number) NOEXCEPT
	{
		return ArctangentRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number, denoted in radians, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArctangentRadians(const float number) NOEXCEPT
	{
		return atanf(number);
	}

	/*
	*	Given two numbers, denoted in degrees, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArctangentDegrees(const float number1, const float number2) NOEXCEPT
	{
		return ArctangentRadians(DegreesToRadians(number1), DegreesToRadians(number2));
	}

	/*
	*	Given two numbers, denoted in radians, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArctangentRadians(const float number1, const float number2) NOEXCEPT
	{
		return atan2f(number1, number2);
	}

	/*
	*	Rounds a number up to the nearest integer.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type Ceiling(const float number) NOEXCEPT
	{
		return static_cast<Type>(number + 1.0f);
	}

	/*
	*	Clamps a value between a lower and an upper limit and returns the clamped value.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type Clamp(const Type value, const Type lowerLimit, const Type upperLimit) NOEXCEPT
	{
		return value < lowerLimit ? lowerLimit : value > upperLimit ? upperLimit : value;
	}

	/*
	*	Given a degrees value, returns the cosine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float CosineDegrees(const float number) NOEXCEPT
	{
		return CosineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a radians value, returns the cosine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float CosineRadians(const float number) NOEXCEPT
	{
		float temporary{ WrapAround(number, -CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI) };

		return	1.0f
				- PowerOf(temporary, 2) * InverseFactorial(2)
				+ PowerOf(temporary, 4) * InverseFactorial(4)
				- PowerOf(temporary, 6) * InverseFactorial(6)
				+ PowerOf(temporary, 8) * InverseFactorial(8)
				- PowerOf(temporary, 10) * InverseFactorial(10)
				+ PowerOf(temporary, 12) * InverseFactorial(12)
				- PowerOf(temporary, 14) * InverseFactorial(14)
				+ PowerOf(temporary, 16) * InverseFactorial(16);
	}

	/*
	*	Given a number in degrees, return it in radians.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float DegreesToRadians(const float number) NOEXCEPT
	{
		return number * CatalystBaseMathConstants::DEGREES_TO_RADIANS;
	}

	/*
	*	Given an integral number, return the factorial of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD uint64 Factorial(const uint8 number) NOEXCEPT
	{
		return number != static_cast<uint8>(0) ? number * Factorial(number - static_cast<uint8>(1)) : static_cast<uint8>(1);
	}

	/*
	*	Rounds a number down to the nearest integer.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type Floor(const float number) NOEXCEPT
	{
		return number >= 0.0f ? static_cast<Type>(static_cast<int32>(number)) : static_cast<Type>(static_cast<int32>(number - 1.0f));
	}

	/*
	*	Given a float, return the fractional part of that float.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Fractional(const float number) NOEXCEPT
	{
		return number - static_cast<float>(static_cast<int32>(number));
	}

	/*
	*	Given an integral number, return the inverse factorial of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float InverseFactorial(const uint8 number) NOEXCEPT
	{
		return 1.0f / static_cast<float>(Factorial(number));
	}

	/*
	*	Returns whether or not an integer is even or not.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type IsEven(const Type number) NOEXCEPT
	{
		return Modulo(number, 2) == 0;
	}

	/*
	*	Returns whether or not a floating point number is NaN (not a number).
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool IsNan(const float number) NOEXCEPT
	{
		return !(number == number);
	}

	/*
	*	Returns whether or not an integer is odd or not.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type IsOdd(const Type number) NOEXCEPT
	{
		return Modulo(number, 2) != 0;
	}

	/*
	*	Linearly interpolate between two float values.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float LinearlyInterpolate(const float valueA, const float valueB, const float alpha) NOEXCEPT
	{
		return (valueA * (1.0f - alpha)) + (valueB * alpha);
	}

	/*
	*	Returns the maximum of two numbers.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type Maximum(const Type firstNumber, const Type secondNumber) NOEXCEPT
	{
		return LIKELY(firstNumber > secondNumber) ? firstNumber : secondNumber;
	}

	/*
	*	Returns the minimum of two numbers.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type Minimum(const Type firstNumber, const Type secondNumber) NOEXCEPT
	{
		return LIKELY(firstNumber < secondNumber) ? firstNumber : secondNumber;
	}

	/*
	*	Returns the modulo of two integers.
	*/
	template <typename First, typename Second>
	FORCE_INLINE constexpr static NO_DISCARD First Modulo(const First input, const Second ceiling) NOEXCEPT
	{
		return UNLIKELY(input >= ceiling) ? input % ceiling : input;
	}

	/*
	*	Given a number and an exponent, returns the power of the exponent.
	*	 Builds on the assumption that the exponent will always be non-zero.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float PowerOf(const float number, const uint8 exponent) NOEXCEPT
	{
		if (UNLIKELY(exponent == 0))
		{
			return 1.0f;
		}

		const float temporary{ PowerOf(number, exponent >> 1) };

		if (IsEven(exponent))
		{
			return temporary * temporary;
		}

		else
		{
			return number * temporary * temporary;
		}
	}

	/*
	*	Given a number in radians, return it in degrees.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float RadiansToDegrees(const float number) NOEXCEPT
	{
		return number * CatalystBaseMathConstants::RADIANS_TO_DEGREES;
	}

	/*
	*	Rounds a float to the nearest integral value, with halfway cases rounded away from zero.
	*/
	template <typename Type>
	FORCE_INLINE constexpr static NO_DISCARD Type Round(const float value) NOEXCEPT
	{
		return value >= 0.0f ? static_cast<Type>(static_cast<int32>(value + 0.5f)) : static_cast<Type>(static_cast<int32>(value - 0.5f));
	}

	/*
	*	Scales a value from one range to another.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Scale(const float value, const float originalMinimum, const float originalMaximum, const float newMinimum, const float newMaximum) NOEXCEPT
	{
		return (((value - originalMinimum) * (newMaximum - newMinimum)) / (originalMaximum - originalMinimum)) + newMinimum;
	}

	/*
	*	Given a number, returns the signum of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Signum(const float number) NOEXCEPT
	{
		return static_cast<float>((number > 0.0f) - (number < 0.0f));
	}

	/*
	*	Given a degrees value, returns the sine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float SineDegrees(const float number) NOEXCEPT
	{
		return SineRadians(DegreesToRadians(number));
	}

	/*
	*	Given a radians value, returns the sine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float SineRadians(const float number) NOEXCEPT
	{
		return CosineRadians(number - CatalystBaseMathConstants::HALF_PI);
	}

	/*
	*	Given a number and a number of derivaties, generate a smooth number.
	*/
	template <uint64 Derivatives>
	FORCE_INLINE constexpr static NO_DISCARD float SmoothStep(const float number) NOEXCEPT
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
	FORCE_INLINE constexpr static NO_DISCARD float SquareRoot(float number) NOEXCEPT
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

		float approximation{ (1.0f + number) * 0.5f };
		float target{ 0.0f };

		while (approximation != target)
		{
			target = approximation;
			approximation = (approximation + number / approximation) * 0.5f;
		}

		return ldexp(approximation, exponent / 2);
	}

	/*
	*	Given a number in degrees, return the tangent.
	*/
	FORCE_INLINE static NO_DISCARD float TangentDegrees(const float number) NOEXCEPT
	{
		return TangentRadians(DegreesToRadians(number));
	}

	/*
	*	Given a number in radians, return the tangent.
	*/
	FORCE_INLINE static NO_DISCARD float TangentRadians(const float number) NOEXCEPT
	{
		return tan(number);
	}

	/*
	*	Wraps a number around the given range.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float WrapAround(const float number, const float minimumRange, const float maximumRange) NOEXCEPT
	{
		float temporary{ number };
		const float wholeRange{ maximumRange - minimumRange };

		while (temporary < minimumRange)
		{
			temporary += wholeRange;
		}

		while (temporary > maximumRange)
		{
			temporary -= wholeRange;
		}

		return temporary;
	}

};