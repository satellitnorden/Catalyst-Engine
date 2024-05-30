#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

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
	*	Default destructor.
	*/
	FORCE_INLINE ~StreamArchive() NOEXCEPT
	{
		if (_Array)
		{
			Memory::Free(_Array);
		}
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
	*	Returns the capacity.
	*/
	FORCE_INLINE NO_DISCARD uint64 Capacity() const NOEXCEPT
	{
		return _Capacity;
	}

	/*
	*	Reads from this stream archive at the given position.
	*	Updates the position.
	*/
	template <typename TYPE>
	FORCE_INLINE void Read(TYPE *const RESTRICT destination, const uint64 size, uint64 *const RESTRICT position) const NOEXCEPT
	{
		Memory::Copy(destination, &_Array[*position], size);

		*position += size;
	}

	/*
	*	Writes to this stream archive.
	*/
	FORCE_INLINE void Write(const void *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		//Calculate the needed memory.
		const uint64 needed_memory{ _Size + size };

		//Re-allocate if we need more memory.
		if (_Capacity < needed_memory)
		{
			Allocate(BaseMath::Maximum<uint64>(_Capacity > 0 ? _Capacity * 2 : 64, needed_memory));
		}

		//Copy the value.
		Memory::Copy(&_Array[_Size], source, size);

		//Update the size.
		_Size += size;
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