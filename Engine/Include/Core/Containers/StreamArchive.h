#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

class StreamArchive final
{

public:

	/*
	*	Enumeration covering all modes.
	*/
	enum class Mode : uint8
	{
		READ,
		WRITE,

		READ_WRITE
	};

	/*
	*	Default constructor.
	*/
	FORCE_INLINE StreamArchive() NOEXCEPT
		:
		_Data(nullptr),
		_Size(0),
		_Capacity(0)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~StreamArchive() NOEXCEPT
	{
		Deallocate();
	}

	/*
	*	Sets the mode.
	*/
	FORCE_INLINE void SetMode(const Mode value) NOEXCEPT
	{
		_Mode = value;
	}

	/*
	*	Sets the data.
	*/
	FORCE_INLINE void SetData(byte *const RESTRICT data, const uint64 size) NOEXCEPT
	{
		Deallocate();

		_Data = data;
		_Size = size;
		_Capacity = size;
		_OwnsData = false;
	}
	
	/*
	*	Returns the underlying data, const.
	*/
	FORCE_INLINE NO_DISCARD const byte *const RESTRICT Data() const NOEXCEPT
	{
		return _Data;
	}

	/*
	*	Returns the underlying data, mutable.
	*/
	FORCE_INLINE NO_DISCARD byte *const RESTRICT Data() NOEXCEPT
	{
		return _Data;
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
		ASSERT(_Mode == Mode::READ || _Mode == Mode::READ_WRITE, "Invalid mode!");

		Memory::Copy(destination, &_Data[*position], size);

		*position += size;
	}

	/*
	*	Writes to this stream archive.
	*/
	FORCE_INLINE void Write(const void *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		ASSERT(_Mode == Mode::WRITE || _Mode == Mode::READ_WRITE, "Invalid mode!");

		//Calculate the needed memory.
		const uint64 needed_memory{ _Size + size };

		//Re-allocate if we need more memory.
		if (_Capacity < needed_memory)
		{
			Allocate(BaseMath::Maximum<uint64>(_Capacity > 0 ? _Capacity * 2 : 64, needed_memory));
		}

		//Copy the value.
		Memory::Copy(&_Data[_Size], source, size);

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

	//The mode.
	Mode _Mode{ Mode::READ_WRITE };

	//The underlying data.
	byte *RESTRICT _Data;

	//The current size of this dynamic array.
	uint64 _Size;

	//The current capacity of this dynamic array.
	uint64 _Capacity;

	//Denotes if this stream archive owns the underlying data.
	bool _OwnsData{ true };

	/*
	*	Allocates.
	*/
	FORCE_INLINE void Allocate(const uint64 capacity) NOEXCEPT
	{
		//Allocate the new data.
		byte *const RESTRICT new_data{ static_cast<byte *const RESTRICT>(Memory::Allocate(capacity)) };

		//Copy over data from the old data and destroy it, if there is one.
		if (_Data)
		{
			Memory::Copy(new_data, _Data, _Size);
			Memory::Free(_Data);
		}

		//Set the new data.
		_Data = new_data;

		//Set the new capacity.
		_Capacity = capacity;

		//This stream archive now owns this data.
		_OwnsData = true;
	}

	/*
	*	Deallocates
	*/
	FORCE_INLINE void Deallocate() NOEXCEPT
	{
		if (_OwnsData && _Data)
		{
			Memory::Free(_Data);
		}

		_Data = nullptr;
		_Size = 0;
		_Capacity = 0;
		_OwnsData = true;
	}

};