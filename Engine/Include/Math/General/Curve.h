#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

template <typename TYPE, uint64 SIZE>
class Curve final
{

public:

	/*
	*	Sets the value at the given index.
	*/
	FORCE_INLINE void SetValue(const uint64 index, const TYPE &value) NOEXCEPT
	{
		_Values[index] = value;
	}

	/*
	*	Samples this curve.
	*/
	FORCE_INLINE NO_DISCARD TYPE Sample(const float32 alpha) NOEXCEPT
	{
		const uint64 first_index{ static_cast<uint64>(alpha * static_cast<float32>(SIZE)) };
		const uint64 second_index{ first_index < (SIZE - 1) ? first_index + 1 : 0 };
		const float32 _alpha{ CatalystBaseMath::SmoothStep<1>(CatalystBaseMath::Fractional(alpha * static_cast<float32>(SIZE))) };

		return CatalystBaseMath::LinearlyInterpolate(_Values[first_index], _Values[second_index], _alpha);
	}

private:

	//The values.
	StaticArray<TYPE, SIZE> _Values;

};