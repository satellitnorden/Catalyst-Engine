#pragma once

/*
*	Clang doesn't have sprintf_s, but instead snprintf, so swap those. Should be fine. (:
*/
#if defined(CATALYST_CLANG)
	#define sprintf_s sprintf
#endif

/*
*	Advances a pointer N bytes.
*/
template <typename TYPE>
FORCE_INLINE static NO_DISCARD TYPE *const RESTRICT AdvancePointer(TYPE *const RESTRICT pointer, const uint64 number_of_bytes) NOEXCEPT
{
	byte *RESTRICT byte_pointer{ static_cast<byte *const RESTRICT>(static_cast<void *const RESTRICT>(pointer)) };
	byte_pointer += number_of_bytes;
	return static_cast<TYPE *const RESTRICT>(static_cast<void *const RESTRICT>(byte_pointer));
}

/*
*	Returns the index of the first set bit in the given integer bit field.
*/
template <typename TYPE>
FORCE_INLINE static NO_DISCARD uint64 BitIndex(const TYPE bit_field) NOEXCEPT
{
	for (uint64 i{ 0 }; i < (sizeof(TYPE) * 8); ++i)
	{
		if (TEST_BIT(bit_field, BIT(i)))
		{
			return i;
		}
	}

	return 0;
}

/*
*	Flips the endian of the given value.
*/
template <typename TYPE>
FORCE_INLINE static void FlipEndian(TYPE *const RESTRICT value) NOEXCEPT
{
	constexpr uint64 SIZE_OF_TYPE{ sizeof(TYPE) };

	union
	{
		TYPE output_value;
		uint8 output_array[SIZE_OF_TYPE];
	} source, destination;
	
	source.output_value = *value;

	for (uint64 i{ 0 }; i < SIZE_OF_TYPE; ++i)
	{
		destination.output_array[i] = source.output_array[SIZE_OF_TYPE - i - 1];
	}

	*value = destination.output_value;
}

/*
*	Flips the endian of the given value and the given size.
*/
FORCE_INLINE static void FlipEndian(void *const RESTRICT value, const uint64 size) NOEXCEPT
{
	//Flipping endiands only make sense in the 1-8 byte range, so assume that's what we want.
	switch (size)
	{
		case 1:
		{
			FlipEndian(static_cast<uint8 *const RESTRICT>(value));

			break;
		}

		case 2:
		{
			FlipEndian(static_cast<uint16 *const RESTRICT>(value));

			break;
		}

		case 4:
		{
			FlipEndian(static_cast<uint32 *const RESTRICT>(value));

			break;
		}

		case 8:
		{
			FlipEndian(static_cast<uint64 *const RESTRICT>(value));

			break;
		}

		default:
		{
			//This is bad, but these functions should preferably be ASSERT-free. (:

			break;
		}
	}
}

/*
*	Returns the number of bits set in the given value.
*/
template <typename TYPE>
FORCE_INLINE static NO_DISCARD uint8 NumberOfBitsSet(const TYPE value) NOEXCEPT
{
	//Define constants.
	constexpr uint64 NUMBER_OF_BYTES{ sizeof(TYPE) };
	constexpr uint64 NUMBER_OF_BITS{ NUMBER_OF_BYTES * 8 };

	//Count the number of bits set.
	uint8 number_of_bits_set{ 0 };

	for (uint64 i{ 0 }; i < NUMBER_OF_BITS; ++i)
	{
		number_of_bits_set += static_cast<TYPE>(TEST_BIT(value, static_cast<TYPE>(BIT(i))));
	}

	//Return the number of bits set.
	return number_of_bits_set;
}

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