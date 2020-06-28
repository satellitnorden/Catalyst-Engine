#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Catalyst base math constants.
namespace CatalystBaseMathConstants
{
	constexpr float32 DEGREES_TO_RADIANS{ 0.017'453f };
	constexpr float32 PI{ 3.141'592f };
	constexpr float32 DOUBLE_PI{ PI * 2.0f };
	constexpr float32 HALF_PI{ PI * 0.5f };
	constexpr float32 QUARTER_PI{ HALF_PI * 0.5f };
	constexpr float32 EIGHTH_PI{ QUARTER_PI * 0.5f };
	constexpr float32 SIXTEENTH_PI{ EIGHTH_PI * 0.5f };
	constexpr float32 THIRTYSECONDTH_PI{ SIXTEENTH_PI * 0.5f };
	constexpr float32 INVERSE_PI{ 1.0f / PI };
	constexpr float32 EULERS_NUMBER{ 2.718281f };
	constexpr float32 PHI{ 1.618033f };
	constexpr float32 RADIANS_TO_DEGREES{ 57.295'779f };
}

class CatalystBaseMath final
{

public:

	/*
	*	Returns the absolute value of the floating point number passed in.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Absolute(const TYPE value) NOEXCEPT
	{
		return value >= static_cast<TYPE>(0) ? value : value * static_cast<TYPE>(-1);
	}

	/*
	*	Given a number, returns the arcsine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float ArcCosine(const float number) NOEXCEPT
	{
		return acosf(number);
	}

	/*
	*	Given a number, returns the arcsine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float Arcsine(const float number) NOEXCEPT
	{
		return asinf(number);
	}

	/*
	*	Given a number, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float Arctangent(const float number) NOEXCEPT
	{
		return atanf(number);
	}

	/*
	*	Given two numbers, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float Arctangent(const float number1, const float number2) NOEXCEPT
	{
		return atan2f(number1, number2);
	}

	/*
	*	Given a value in the range [0.0f, 1.0f] and a bias in the range [0.0f, 1.0f],
	*	biases the value either towards 0.0f if bias is < 0.5f or towards 1.0f if bias is >= 0.5f.
	*/
	FORCE_INLINE static NO_DISCARD float Bias(const float value, const float bias) NOEXCEPT
	{
		return bias >= 0.5f ? LinearlyInterpolate(value, InverseSquare(value), (bias - 0.5f) * 2.0f) : LinearlyInterpolate(value, Square(value), bias * 2.0f);
	}

	/*
	*	Rounds a number up to the nearest integer.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Ceiling(const float number) NOEXCEPT
	{
		return number >= 0.0f ? static_cast<TYPE>(static_cast<int32>(number + 1.0f)) : static_cast<TYPE>(static_cast<int32>(number));
	}

	/*
	*	Clamps a value between a lower and an upper limit and returns the clamped value.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Clamp(const TYPE value, const TYPE lower, const TYPE upper) NOEXCEPT
	{
		return value < lower ? lower : value > upper ? upper : value;
	}

	/*
	*	Given an angle, returns the cosecant of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Cosecant(const float angle) NOEXCEPT
	{
		return 1.0f / Sine(angle);
	}

	/*
	*	Given an angle, returns the cosine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Cosine(const float angle) NOEXCEPT
	{
		const float temporary{ WrapAround(angle, -CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI) };

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
	*	Given an angle, returns the cotangent of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Cotangent(const float angle) NOEXCEPT
	{
		return Cosine(angle) / Sine(angle);
	}

	/*
	*	Given a number in degrees, return it in radians.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float DegreesToRadians(const float number) NOEXCEPT
	{
		return number * CatalystBaseMathConstants::DEGREES_TO_RADIANS;
	}

	/*
	*	Generates the exponential of a number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Exponential(const float number) NOEXCEPT
	{
		return	1.0f
				+ number
				+ PowerOf(number, 2) * InverseFactorial(2)
				+ PowerOf(number, 3) * InverseFactorial(3)
				+ PowerOf(number, 4) * InverseFactorial(4)
				+ PowerOf(number, 5) * InverseFactorial(5)
				+ PowerOf(number, 6) * InverseFactorial(6)
				+ PowerOf(number, 7) * InverseFactorial(7)
				+ PowerOf(number, 3) * InverseFactorial(3);
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
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Floor(const float number) NOEXCEPT
	{
		return number >= 0.0f ? static_cast<TYPE>(static_cast<int32>(number)) : static_cast<TYPE>(static_cast<int32>(number - 1.0f));
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
	*	Given a number, returns the inverse square of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float InverseSquare(const float number) NOEXCEPT
	{
		return 1.0f - Square(1.0f - number);
	}

	/*
	*	Returns whether or not an integer is even or not.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE IsEven(const TYPE number) NOEXCEPT
	{
		return Modulo(number, 2) == 0;
	}

	/*
	*	Returns whether or not a floating point number is NaN (not a number).
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool IsNaN(const float number) NOEXCEPT
	{
		return !(number == number);
	}

	/*
	*	Returns whether or not an integer is odd or not.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE IsOdd(const TYPE number) NOEXCEPT
	{
		return Modulo(number, 2) != 0;
	}

	/*
	*	Returns whether or not a number is a power of two.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD bool IsPowerOfTwo(const TYPE number) NOEXCEPT
	{
		return (number & (number - 1)) == static_cast<TYPE>(0);
	}

	/*
	*	Linearly interpolate between two float values.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE LinearlyInterpolate(const TYPE A, const TYPE B, const float alpha) NOEXCEPT
	{
		return (A * (1.0f - alpha)) + (B * alpha);
	}

	/*
	*	Returns the maximum of two numbers.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Maximum(const TYPE first, const TYPE second) NOEXCEPT
	{
		return LIKELY(first > second) ? first : second;
	}

	/*
	*	Returns the minimum of two numbers.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Minimum(const TYPE first, const TYPE second) NOEXCEPT
	{
		return LIKELY(first < second) ? first : second;
	}

	/*
	*	Returns the modulo of two integers.
	*/
	template <typename FIRST, typename SECOND>
	FORCE_INLINE constexpr static NO_DISCARD FIRST Modulo(const FIRST input, const SECOND ceiling) NOEXCEPT
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
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Round(const float value) NOEXCEPT
	{
		return value >= 0.0f ? static_cast<TYPE>(static_cast<int32>(value + 0.5f)) : static_cast<TYPE>(static_cast<int32>(value - 0.5f));
	}

	/*
	*	Rounds the given uint32 up to the nearest power of two.
	*/
	FORCE_INLINE constexpr static NO_DISCARD uint32 RoundUpToNearestPowerOfTwo(const uint32 value) NOEXCEPT
	{
		uint32 output{ value };

		--output;
		output |= output >> 1;
		output |= output >> 2;
		output |= output >> 4;
		output |= output >> 8;
		output |= output >> 16;
		++output;

		return output;
	}


	/*
	*	Scales a value from one range to another.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Scale(const float value, const float originalMinimum, const float originalMaximum, const float newMinimum, const float newMaximum) NOEXCEPT
	{
		return (((value - originalMinimum) * (newMaximum - newMinimum)) / (originalMaximum - originalMinimum)) + newMinimum;
	}

	/*
	*	Given an angle, returns the secant of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Secant(const float angle) NOEXCEPT
	{
		return 1.0f / Cosine(angle);
	}

	/*
	*	Given a number, returns the signum of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Signum(const float number) NOEXCEPT
	{
		return static_cast<float>((number > 0.0f) - (number < 0.0f));
	}

	/*
	*	Given an angle, returns the sine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Sine(const float angle) NOEXCEPT
	{
		/*
		float temporary{ WrapAround(angle, -CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI) };

		return	temporary
				- PowerOf(temporary, 3) * InverseFactorial(3)
				+ PowerOf(temporary, 5) * InverseFactorial(5)
				- PowerOf(temporary, 7) * InverseFactorial(7)
				+ PowerOf(temporary, 9) * InverseFactorial(9)
				- PowerOf(temporary, 11) * InverseFactorial(11)
				+ PowerOf(temporary, 13) * InverseFactorial(13)
				- PowerOf(temporary, 15) * InverseFactorial(15)
				+ PowerOf(temporary, 17) * InverseFactorial(17);
		*/

		/*
		*	Due to the implementation of Sine/Cosine (using Taylor Series), it's slightly cheaper
		*	to redirect Sine to Cosine since it uses slighlty lower exponents for PowerOf().
		*/
		return Cosine(angle - CatalystBaseMathConstants::HALF_PI);
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
	*	Given a number, returns the square of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Square(const float number) NOEXCEPT
	{
		return number * number;
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
	*	Given an angle, return the tangent of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float Tangent(const float angle) NOEXCEPT
	{
		return Sine(angle) / Cosine(angle);
	}

	/*
	*	Wraps a number around the given range.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float WrapAround(const float number, const float minimumRange, const float maximumRange) NOEXCEPT
	{
		float temporary{ number };
		const float whole_range{ maximumRange - minimumRange };

		while (temporary < minimumRange)
		{
			temporary += whole_range;
		}

		while (temporary > maximumRange)
		{
			temporary -= whole_range;
		}

		return temporary;
	}

};