#pragma once

/*
*	Decrements a float32 down towards negative infinity to the previous representable value.
*/
FORCE_INLINE static NO_DISCARD float32 PreviousFloat32(const float32 number) NOEXCEPT
{
	//Copy the bits of the number.
	uint32 bits;
	Memory::Copy(&bits, &number, sizeof(uint32));

	//Perform the increment/decrement.
	if (number >= 0.0f) --bits;
	else ++bits;

	//Copy the return value back.
	float32 return_value;
	Memory::Copy(&return_value, &bits, sizeof(float32));

	return return_value;
}

/*
*	Increments a float32 up towards positive infinity to the next representable value.
*/
FORCE_INLINE static NO_DISCARD float32 NextFloat32(const float32 number) NOEXCEPT
{
	//Copy the bits of the number.
	uint32 bits;
	Memory::Copy(&bits, &number, sizeof(uint32));

	//Perform the increment/decrement.
	if (number >= 0.0f) ++bits;
	else --bits;

	//Copy the return value back.
	float32 return_value;
	Memory::Copy(&return_value, &bits, sizeof(float32));

	return return_value;
}


/*
*	Decrements a float64 down towards negative infinity to the previous representable value.
*/
FORCE_INLINE static NO_DISCARD float64 PreviousFloat64(const float64 number) NOEXCEPT
{
	//Copy the bits of the number.
	uint64 bits;
	Memory::Copy(&bits, &number, sizeof(uint64));

	//Perform the increment/decrement.
	if (number >= 0.0) --bits;
	else ++bits;

	//Copy the return value back.
	float64 return_value;
	Memory::Copy(&return_value, &bits, sizeof(float64));

	return return_value;
}

/*
*	Increments a float64 up towards positive infinity to the next representable value.
*/
FORCE_INLINE static NO_DISCARD float64 NextFloat64(const float64 number) NOEXCEPT
{
	//Copy the bits of the number.
	uint64 bits;
	Memory::Copy(&bits, &number, sizeof(uint64));

	//Perform the increment/decrement.
	if (number >= 0.0f) ++bits;
	else --bits;

	//Copy the return value back.
	float64 return_value;
	Memory::Copy(&return_value, &bits, sizeof(float64));

	return return_value;
}

/*
*	Swaps two elements.
*/
template <typename TYPE>
FORCE_INLINE constexpr static void Swap(TYPE *const RESTRICT first, TYPE *const RESTRICT second) NOEXCEPT
{
	TYPE temporary{ std::move(*first) };
	*first = std::move(*second);
	*second = std::move(temporary);
}