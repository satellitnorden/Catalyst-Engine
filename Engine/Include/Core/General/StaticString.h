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
		constexpr char EMPTY_STRING[]{ "" };
		constexpr uint64 EMPTY_STRING_LENGTH{ StringUtilities::StringLength(EMPTY_STRING) };

		Memory::Copy(_Data.Data(), EMPTY_STRING, EMPTY_STRING_LENGTH + 1);
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
	*	Returns the underlying data.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char* const RESTRICT Data() NOEXCEPT
	{
		return _Data.Data();
	}

private:

	//The underlying data.
	StaticArray<char, SIZE> _Data{  };

};