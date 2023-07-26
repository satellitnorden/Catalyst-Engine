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
	FORCE_INLINE static NO_DISCARD float32 ArcCosine(const float32 number) NOEXCEPT
	{
		return acosf(number);
	}

	/*
	*	Given a number, returns the arcsine of the number.
	*/
	FORCE_INLINE static NO_DISCARD float32 ArcSine(const float32 number) NOEXCEPT
	{
		return asinf(number);
	}

	/*
	*	Given a number, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float32 ArcTangent(const float32 number) NOEXCEPT
	{
		return atanf(number);
	}

	/*
	*	Given two numbers, returns the arctangent of the number.
	*/
	FORCE_INLINE static NO_DISCARD float32 ArcTangent(const float32 Y, const float32 X) NOEXCEPT
	{
		return atan2f(Y, X);
	}

	/*
	*	Given a value in the range [0.0f, 1.0f] and a bias in the range [0.0f, 1.0f],
	*	biases the value either towards 0.0f if bias is < 0.5f or towards 1.0f if bias is >= 0.5f.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE Bias(const TYPE value, const TYPE bias) NOEXCEPT
	{
		if (bias >= static_cast<TYPE>(0.5))
		{
			const TYPE bias_alpha{ (bias - static_cast<TYPE>(0.5)) * static_cast<TYPE>(2) };

			return (value * (static_cast<TYPE>(1) - bias_alpha)) + (InverseSquare(value) * bias_alpha);
		}

		else
		{
			const TYPE bias_alpha{ static_cast<TYPE>(1) - (bias * static_cast<TYPE>(2)) };

			return (value * (static_cast<TYPE>(1) - bias_alpha)) + (Square(value) * bias_alpha);
		}
	}

	/*
	*	Rounds a number up to the nearest integer.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Ceiling(const float32 number) NOEXCEPT
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
	FORCE_INLINE constexpr static NO_DISCARD float32 Cosecant(const float32 angle) NOEXCEPT
	{
		return 1.0f / Sine(angle);
	}

	/*
	*	Given an angle, returns the cosine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Cosine(const float32 angle) NOEXCEPT
	{
		const float32 temporary{ WrapAround(angle, -CatalystBaseMathConstants::PI, CatalystBaseMathConstants::PI) };

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
	FORCE_INLINE constexpr static NO_DISCARD float32 Cotangent(const float32 angle) NOEXCEPT
	{
		return Cosine(angle) / Sine(angle);
	}

	/*
	*	Returns the cube root of the given number.
	*/
	FORCE_INLINE static NO_DISCARD float32 CubeRoot(const float32 number) NOEXCEPT
	{
		return std::cbrtf(number);
	}

	/*
	*	Given a number in degrees, return it in radians.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 DegreesToRadians(const float32 number) NOEXCEPT
	{
		return number * CatalystBaseMathConstants::DEGREES_TO_RADIANS;
	}

	/*
	*	Generates the exponential of a number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Exponential(const float32 number) NOEXCEPT
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
	FORCE_INLINE constexpr static NO_DISCARD TYPE Floor(const float32 number) NOEXCEPT
	{
		return number >= 0.0f ? static_cast<TYPE>(static_cast<int32>(number)) : static_cast<TYPE>(static_cast<int32>(number - 1.0f));
	}

	/*
	*	Given a float32, return the fractional part of that float32.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Fractional(const float32 number) NOEXCEPT
	{
		return number - Floor<float32>(number);
	}

	/*
	*	Given an integral number, return the inverse factorial of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 InverseFactorial(const uint8 number) NOEXCEPT
	{
		return 1.0f / static_cast<float32>(Factorial(number));
	}

	/*
	*	Given a number, returns the inverse square of that number.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE InverseSquare(const TYPE number) NOEXCEPT
	{
		return static_cast<TYPE>(1) - Square(static_cast<TYPE>(1) - number);
	}

	/*
	*	Given a number, returns the inverse square root.
	*/
	FORCE_INLINE static NO_DISCARD float32 InverseSquareRoot(const float32 number) NOEXCEPT
	{
		//Define constants.
		constexpr float32 THREE_HALFS{ 1.5f };

		//Calculate the half number.
		const float32 half_number{ number * 0.5f };

		//Do the evil bit hack.
		float32 final_number{ number };

		int32 integer{ *(int32*)&final_number };
		integer = 0x5f3759df - (integer >> 1);
		final_number = *(float32*)&integer;

		//Perform the Newton iteration.
		final_number = final_number * (THREE_HALFS - (half_number * final_number * final_number));
		final_number = final_number * (THREE_HALFS - (half_number * final_number * final_number)); //Second iteration, can be skipped, tradeoff being less accuracy.

		//Return the final number!
		return final_number;
	}

	/*
	*	Returns whether or not an integer is even or not.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD bool IsEven(const TYPE number) NOEXCEPT
	{
		return (number & static_cast<TYPE>(1)) == 0;
	}

	/*
	*	Returns if a given integer is a multiple of another integer.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD bool IsMultipleOf(const TYPE number, const TYPE multiple) NOEXCEPT
	{
		return Modulo(number, multiple) == static_cast<TYPE>(0);
	}

	/*
	*	Returns whether or not a floating point number is NaN (not a number).
	*/
	FORCE_INLINE constexpr static NO_DISCARD bool IsNaN(const float32 number) NOEXCEPT
	{
		return !(number == number);
	}

	/*
	*	Returns whether or not an integer is odd or not.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD bool IsOdd(const TYPE number) NOEXCEPT
	{
		return (number & static_cast<TYPE>(1)) == 1;
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
	*	Linearly interpolate between two values.
	*/
	template <typename TYPE_TYPE, typename ALPHA_TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE_TYPE LinearlyInterpolate(const TYPE_TYPE A, const TYPE_TYPE B, const ALPHA_TYPE alpha) NOEXCEPT
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
	*	Builds on the assumption that the exponent will always be non-zero.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 PowerOf(const float32 number, const uint8 exponent) NOEXCEPT
	{
		if (UNLIKELY(exponent == 0))
		{
			return 1.0f;
		}

		const float32 temporary{ PowerOf(number, exponent >> 1) };

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
	FORCE_INLINE constexpr static NO_DISCARD float32 RadiansToDegrees(const float32 number) NOEXCEPT
	{
		return number * CatalystBaseMathConstants::RADIANS_TO_DEGREES;
	}

	/*
	*	Rounds a float32 to the nearest integral value, with halfway cases rounded away from zero.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Round(const float32 value) NOEXCEPT
	{
		return value >= 0.0f ? static_cast<TYPE>(static_cast<int32>(value + 0.5f)) : static_cast<TYPE>(static_cast<int32>(value - 0.5f));
	}

	/*
	*	Rounds the given uint32 down to the nearest power of two.
	*/
	FORCE_INLINE constexpr static NO_DISCARD uint32 RoundDownToNearestPowerOfTwo(const uint32 value) NOEXCEPT
	{
		uint32 output{ value };

		output |= output >> 1;
		output |= output >> 2;
		output |= output >> 4;
		output |= output >> 8;
		output |= output >> 16;

		return output - (output >> 1);
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
	*	Rounds the given integer number to the nearest multiple of the given multiple.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE RoundToNearestMultipleOf(const TYPE number, const TYPE multiple) NOEXCEPT
	{
		return ((number + multiple / 2) / multiple) * multiple;
	}

	/*
	*	Scales a value from one range to another.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Scale(const float32 value, const float32 originalMinimum, const float32 originalMaximum, const float32 newMinimum, const float32 newMaximum) NOEXCEPT
	{
		return (((value - originalMinimum) * (newMaximum - newMinimum)) / (originalMaximum - originalMinimum)) + newMinimum;
	}

	/*
	*	Given an angle, returns the secant of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Secant(const float32 angle) NOEXCEPT
	{
		return 1.0f / Cosine(angle);
	}

	/*
	*	Given a number, returns the signum of that number.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Signum(const float32 number) NOEXCEPT
	{
		return static_cast<float32>((number > 0.0f) - (number < 0.0f));
	}

	/*
	*	Given an angle, returns the sine of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Sine(const float32 angle) NOEXCEPT
	{
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
	FORCE_INLINE constexpr static NO_DISCARD float32 SmoothStep(const float32 number) NOEXCEPT
	{
		float32 smoothedNumber = number;

		for (uint64 i = 0; i < Derivatives; ++i)
		{
			smoothedNumber = smoothedNumber * smoothedNumber * (3.0f - 2.0f * smoothedNumber);
		}

		return smoothedNumber;
	}

	/*
	*   Returns a smoothed number in the given range.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 SmoothStep(const float32 number, const float32 minimum, const float32 maximum) NOEXCEPT
	{
		const float32 clamped_number{ Clamp((number - minimum) / (maximum - minimum), 0.0f, 1.0f) };

		return clamped_number * clamped_number * (3.0f - 2.0f * clamped_number);
	}

	/*
	*	Given a number, returns the square of that number.
	*/
	template <typename TYPE>
	FORCE_INLINE constexpr static NO_DISCARD TYPE Square(const TYPE number) NOEXCEPT
	{
		return number * number;
	}

	/*
	*	Given a number, returns the square root.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 SquareRoot(float32 number) NOEXCEPT
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

	/*
	*	Given an angle, return the tangent of the angle.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 Tangent(const float32 angle) NOEXCEPT
	{
		return Sine(angle) / Cosine(angle);
	}

	/*
	*	Wraps a number around the given range.
	*/
	FORCE_INLINE constexpr static NO_DISCARD float32 WrapAround(const float32 number, const float32 minimumRange, const float32 maximumRange) NOEXCEPT
	{
		float32 temporary{ number };
		const float32 whole_range{ maximumRange - minimumRange };

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