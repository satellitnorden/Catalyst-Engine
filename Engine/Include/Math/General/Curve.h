#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/BaseMath.h>

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
		const uint64 first_index{ BaseMath::Minimum<uint64>(static_cast<uint64>(alpha * static_cast<float32>(SIZE - 1)), SIZE - 1) };
		const uint64 second_index{ BaseMath::Minimum<uint64>(first_index + 1, SIZE - 1) };
		const float32 _alpha{ BaseMath::Fractional(alpha * static_cast<float32>(SIZE - 1)) };

		return BaseMath::LinearlyInterpolate(_Values[first_index], _Values[second_index], _alpha);
	}

private:

	//The values.
	StaticArray<TYPE, SIZE> _Values;

};