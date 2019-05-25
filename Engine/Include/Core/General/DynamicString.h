#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class DynamicString final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DynamicString() NOEXCEPT
		:
		_String(nullptr),
		_Length(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE DynamicString(const DynamicString &otherString) NOEXCEPT
	{
		//Set the length of this string.
		_Length = otherString.Length();

		//Allocate sufficient memory for the underlying string.
		_String = static_cast<char *RESTRICT>(Memory::AllocateMemory(_Length + 1));

		//Copy the contents of the other string.
		Memory::CopyMemory(_String, otherString._String, _Length + 1);
	}

	/*
	*	Move constructor.
	*/
	FORCE_INLINE DynamicString(DynamicString &&otherString) NOEXCEPT
	{
		//Steal the other string's underlying string and set it to nullptr!
		_String = otherString._String;
		otherString._String = nullptr;

		//Steal the other string's length!
		_Length = otherString._Length;
	}

	/*
	*	Constructor taking a C string.
	*/
	FORCE_INLINE DynamicString(const char *const RESTRICT newString) NOEXCEPT
	{
		//Determine how long the C string is.
		_Length = strlen(newString);

		//Allocate sufficient memory to host the string.
		_String = static_cast<char *RESTRICT>(Memory::AllocateMemory(_Length + 1));

		//Copy the string to the memory.
		Memory::CopyMemory(static_cast<void *RESTRICT>(_String), static_cast<const void *const RESTRICT>(newString), _Length + 1);
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~DynamicString() NOEXCEPT
	{
		//Free the underlying string.
		Memory::FreeMemory(_String);
	}

	/*
	*	Copy assignment operator overload.
	*/
	FORCE_INLINE void operator=(const DynamicString &otherString) NOEXCEPT
	{
		//Update the length of this string.
		_Length = otherString.Length();

		//Reallocate sufficient memory for the underlying string.
		_String = static_cast<char *RESTRICT>(Memory::ReallocateMemory(static_cast<void *RESTRICT>(_String), _Length + 1));

		//Copy the contents of the other string.
		Memory::CopyMemory(_String, otherString._String, _Length + 1);
	}

	/*
	*	Move assignment operator overload.
	*/
	FORCE_INLINE void operator=(DynamicString &&otherString) NOEXCEPT
	{
		//Steal the other string's underlying string and set it to nullptr!
		_String = otherString._String;
		otherString._String = nullptr;

		//Steal the other string's length!
		_Length = otherString._Length;
	}

	/*
	*	Addition operator overload.
	*/
	FORCE_INLINE NO_DISCARD DynamicString operator+(const char *const RESTRICT newString) const NOEXCEPT
	{
		//Construct the new dynamic string.
		DynamicString newDynamicString;

		//Determine how long the new dynamic string will be.
		uint64 newStringLength{ strlen(newString) };
		uint64 newLength{ this->_Length + newStringLength };

		//Allocate sufficient memory to host the string.
		newDynamicString._String = static_cast<char *RESTRICT>(Memory::AllocateMemory(newLength + 1));

		//Copy the original string to the memory.
		Memory::CopyMemory(static_cast<void *RESTRICT>(newDynamicString._String), static_cast<const void *const RESTRICT>(this->_String), this->_Length);

		//Copy the new string to the memory.
		Memory::CopyMemory(static_cast<void *RESTRICT>(newDynamicString._String + this->_Length), static_cast<const void *const RESTRICT>(newString), newStringLength + 1);

		//Return the newly constructed dynamic string.
		return newDynamicString;
	}

	/*
	*	Addition assignment operator overload.
	*/
	FORCE_INLINE void operator+=(const char *const RESTRICT newString) NOEXCEPT
	{
		//Calculate the new length.
		const uint64 newStringLength = strlen(newString) + 1;
		const uint64 newLength = _Length + newStringLength - 1;

		//Allocate sufficient memory to host the concatenated string.
		_String = static_cast<char *RESTRICT>(Memory::ReallocateMemory(static_cast<void *RESTRICT>(_String), newLength + 1));

		//Copy the new string.
		Memory::CopyMemory(_String + _Length, newString, newStringLength);

		//Update the length of the string.
		_Length = newLength;
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT Begin() const  NOEXCEPT
	{
		return _String;
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT Begin()  NOEXCEPT
	{
		return _String;
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT End() const NOEXCEPT
	{
		return _String + _Length;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT End() NOEXCEPT
	{
		return _String + _Length;
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT begin() const  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT begin()  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT end() const NOEXCEPT
	{
		return End();
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD char *const RESTRICT end() NOEXCEPT
	{
		return End();
	}

	/*
	*	Returns the underlying data.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const char *const RESTRICT Data() const NOEXCEPT
	{
		return _String;
	}

	/*
	*	Returns the length of the string.
	*/
	FORCE_INLINE NO_DISCARD uint64 Length() const NOEXCEPT
	{
		return _Length;
	}

private:

	//The underlying C string.
	char *RESTRICT _String;

	//The length of the string.
	uint64 _Length;

};