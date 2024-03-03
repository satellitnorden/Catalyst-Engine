#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class StreamArchive final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE StreamArchive() NOEXCEPT
		:
		_Array(nullptr),
		_Size(0),
		_Capacity(0)
	{

	}
	
	/*
	*	Returns the underlying data, const.
	*/
	FORCE_INLINE NO_DISCARD const byte *const RESTRICT Data() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the underlying data, mutable.
	*/
	FORCE_INLINE NO_DISCARD byte *const RESTRICT Data() NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the size.
	*/
	FORCE_INLINE NO_DISCARD uint64 Size() const NOEXCEPT
	{
		return _Size;
	}

	/*
	*	Writes to this stream archive.
	*/
	template <typename TYPE>
	FORCE_INLINE void Write(const TYPE &value) NOEXCEPT
	{
		//Define constants.
		constexpr uint64 SIZE_OF_TYPE{ sizeof(TYPE) };

		//Calculate the needed memory.
		const uint64 needed_memory{ _Size + SIZE_OF_TYPE };

		//Re-allocate if we need more memory.
		if (_Capacity < needed_memory)
		{
			Allocate(needed_memory);
		}

		//Copy the value.
		Memory::Copy(&_Array[_Size], &value, SIZE_OF_TYPE);

		//Update the size.
		_Size += SIZE_OF_TYPE;
	}

	/*
	*	Reads from this stream archive at the given position.
	*/
	FORCE_INLINE NO_DISCARD const byte *const RESTRICT Read(const uint64 position) const NOEXCEPT
	{
		return &_Array[position];
	}

	/*
	*	Resizes this stream archive.
	*/
	FORCE_INLINE NO_DISCARD void Resize(const uint64 size) NOEXCEPT
	{
		Allocate(size);
		_Size = size;
	}

private:

	//The underlying array.
	byte *RESTRICT _Array;

	//The current size of this dynamic array.
	uint64 _Size;

	//The current capacity of this dynamic array.
	uint64 _Capacity;

	/*
	*	Allocates.
	*/
	void Allocate(const uint64 capacity) NOEXCEPT
	{
		//Allocate the new array.
		byte *const RESTRICT new_array{ static_cast<byte *const RESTRICT>(Memory::Allocate(capacity)) };

		//Copy over data from the old array and destroy it, if there is one.
		if (_Array)
		{
			Memory::Copy(new_array, _Array, _Size);
			Memory::Free(_Array);
		}

		//Set the new array.
		_Array = new_array;

		//Set the new capacity.
		_Capacity = capacity;
	}

};