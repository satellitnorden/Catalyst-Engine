#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

class TimeSignature final
{

public:

	//The numerator.
	uint8 _Numerator;

	//The denominator.
	uint32 _Denominator;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE TimeSignature() NOEXCEPT
		:
		_Numerator(0),
		_Denominator(0)
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE TimeSignature(const uint8 numerator, const uint32 denominator) NOEXCEPT
		:
		_Numerator(numerator),
		_Denominator(denominator)
	{

	}

	/*
	*	Verifies this time signature.
	*/
	FORCE_INLINE void Verify() const NOEXCEPT
	{
		ASSERT(_Numerator != 0, "Numerator must not be zero!");
		ASSERT(_Denominator != 0, "Denominator must not be zero!");
		ASSERT(BaseMath::IsPowerOfTwo(_Denominator), "Denominator must be power of two!");
	}

	/*
	*	Modifies the numerator to the given (power of two) number.
	*/
	FORCE_INLINE void ConformNumerator(const uint32 denominator) NOEXCEPT
	{
		ASSERT(denominator != 0, "Denominator must not be zero!");
		ASSERT(BaseMath::IsPowerOfTwo(denominator), "Denominator must be power of two!");

		if (_Denominator > denominator)
		{
			while (_Denominator > denominator)
			{
				_Denominator = BaseMath::RoundDownToNearestPowerOfTwo(_Denominator - 1);
				_Numerator *= 2;
			}
		}

		else if (_Denominator < denominator)
		{
			while (_Denominator < denominator)
			{
				_Denominator = BaseMath::RoundUpToNearestPowerOfTwo(_Denominator + 1);
				_Numerator /= 2;
			}
		}
	}

};