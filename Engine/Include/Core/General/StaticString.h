#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Utilities/StringUtilities.h>

template <uint64 SIZE>
class StaticString final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr StaticString() NOEXCEPT
	{
		_Data[0] = '\0';
	}

	/*
	*	Constructor taking a C style string.
	*/
	FORCE_INLINE constexpr StaticString(const char* const RESTRICT string) NOEXCEPT
	{
		const uint64 string_length{ StringUtilities::StringLength(string) };

		Memory::Copy(_Data.Data(), string, string_length + 1);
	}
	
	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE constexpr operator bool() const NOEXCEPT
	{
		return _Data[0] != '\0';
	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE NO_DISCARD const char &operator[](const uint64 index) const NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Subscript operator overload, mutable.
	*/
	FORCE_INLINE NO_DISCARD char &operator[](const uint64 index) NOEXCEPT
	{
		return _Data[index];
	}

	/*
	*	Returns the length.
	*/
	FORCE_INLINE NO_DISCARD uint64 Length() const NOEXCEPT
	{
		return StringUtilities::StringLength(Data());
	}

	/*
	*	Returns the size.
	*/
	FORCE_INLINE constexpr NO_DISCARD uint64 Size() const NOEXCEPT
	{
		return SIZE;
	}

	/*
	*	Returns the underlying data, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT Data() const NOEXCEPT
	{
		return _Data.Data();
	}

	/*
	*	Returns the underlying data, mutable.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT Data() NOEXCEPT
	{
		return _Data.Data();
	}

private:

	//The underlying data.
	StaticArray<char, SIZE> _Data{  };

};