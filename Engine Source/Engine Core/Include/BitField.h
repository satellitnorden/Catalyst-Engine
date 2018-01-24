#pragma once

template <uint8 BitFieldSize>
class BitField final
{

public:

	/*
	*	Default constructor.
	*/
	BitField() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~BitField() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Returns whether or not a bit at a specified index is set.
	*/
	bool IsBitSet(const uint8 index) const CATALYST_NOEXCEPT
	{
		return bitField & (static_cast<uint64>(0x00000001) << index);
	}

	/*
	*	Sets the bit at the given index.
	*/
	void SetBit(const uint8 index) CATALYST_NOEXCEPT
	{
		bitField |= static_cast<uint64>(0x00000001) << index;
	}

	/*
	*	Unsets the bit at the given index.
	*/
	void UnsetBit(const uint8 index) CATALYST_NOEXCEPT
	{
		bitField &= ~(static_cast<uint64>(0x00000001) << index);
	}

	/*
	*	Toggles the bit at the given index.
	*/
	void ToggleBit(const uint8 index) CATALYST_NOEXCEPT
	{
		bitField ^= static_cast<uint64>(0x00000001) << index;
	}

private:

	//The underlying bit field.
	uint64 bitField : BitFieldSize;

};