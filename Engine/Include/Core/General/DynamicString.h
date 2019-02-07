#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/MemoryUtilities.h>

class DynamicString final
{

public:

	/*
	*	Default constructor.
	*/
	DynamicString() NOEXCEPT
		:
		_String(nullptr),
		_Length(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	DynamicString(const DynamicString &otherString) NOEXCEPT
	{
		//Set the length of this string.
		_Length = otherString.Length();

		//Allocate sufficient memory for the underlying string.
		_String = static_cast<char *RESTRICT>(MemoryUtilities::AllocateMemory(_Length + 1));

		//Copy the contents of the other string.
		MemoryUtilities::CopyMemory(_String, otherString._String, _Length + 1);
	}

	/*
	*	Move constructor.
	*/
	DynamicString(DynamicString &&otherString) NOEXCEPT
	{
		//Steal the other string's underlying string and set it to null!
		_String = otherString._String;
		otherString._String = nullptr;

		//Steal the other string's length!
		_Length = otherString._Length;
	}

	/*
	*	Constructor taking a C string.
	*/
	DynamicString(const char *const RESTRICT newString)
	{
		//Determine how long the C string is.
		_Length = strlen(newString);

		//Allocate sufficient memory to host the string.
		_String = static_cast<char *RESTRICT>(MemoryUtilities::AllocateMemory(_Length + 1));

		//Copy the string to the memory.
		MemoryUtilities::CopyMemory(static_cast<void *RESTRICT>(_String), static_cast<const void *const RESTRICT>(newString), _Length + 1);
	}

	/*
	*	Default destructor.
	*/
	~DynamicString() NOEXCEPT
	{
		//Free the underlying string.
		MemoryUtilities::FreeMemory(_String);
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const DynamicString &otherString) NOEXCEPT
	{
		//Update the length of this string.
		_Length = otherString.Length();

		//Reallocate sufficient memory for the underlying string.
		_String = static_cast<char *RESTRICT>(MemoryUtilities::ReallocateMemory(static_cast<void *RESTRICT>(_String), _Length + 1));

		//Copy the contents of the other string.
		MemoryUtilities::CopyMemory(_String, otherString._String, _Length + 1);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(DynamicString &&otherString) NOEXCEPT
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
	DynamicString operator+(const char *const RESTRICT newString) const NOEXCEPT
	{
		//Construct the new dynamic string.
		DynamicString newDynamicString;

		//Determine how long the new dynamic string will be.
		uint64 newStringLength{ strlen(newString) };
		uint64 newLength{ this->_Length + newStringLength };

		//Allocate sufficient memory to host the string.
		newDynamicString._String = static_cast<char *RESTRICT>(MemoryUtilities::AllocateMemory(newLength + 1));

		//Copy the original string to the memory.
		MemoryUtilities::CopyMemory(static_cast<void *RESTRICT>(newDynamicString._String), static_cast<const void *const RESTRICT>(this->_String), this->_Length);

		//Copy the new string to the memory.
		MemoryUtilities::CopyMemory(static_cast<void *RESTRICT>(newDynamicString._String + this->_Length), static_cast<const void *const RESTRICT>(newString), newStringLength + 1);

		//Return the newly constructed dynamic string.
		return newDynamicString;
	}

	/*
	*	Addition assignment operator overload.
	*/
	void operator+=(const char *const RESTRICT newString) NOEXCEPT
	{
		//Calculate the new length.
		const uint64 newStringLength = strlen(newString) + 1;
		const uint64 newLength = _Length + newStringLength - 1;

		//Allocate sufficient memory to host the concatenated string.
		_String = static_cast<char *RESTRICT>(MemoryUtilities::ReallocateMemory(static_cast<void *RESTRICT>(_String), newLength + 1));

		//Copy the new string.
		MemoryUtilities::CopyMemory(_String + _Length, newString, newStringLength);

		//Update the length of the string.
		_Length = newLength;
	}

	/*
	*	Less than operator overload.
	*/
	bool operator<(const DynamicString &otherString) const NOEXCEPT
	{
		return strcmp(this->_String, otherString._String) < 0;
	}

	/*
	*	Returns the underlying C string, const.
	*/
	RESTRICTED const char *const CString() const NOEXCEPT
	{
		return _String;
	}

	/*
	*	Returns the underlying C string, non-const.
	*/
	RESTRICTED char *const CString() NOEXCEPT
	{
		return _String;
	}

	/*
	*	Returns the length of the string.
	*/
	uint64 Length() const NOEXCEPT
	{
		return _Length;
	}

private:

	//The underlying C string.
	char *RESTRICT _String;

	//The length of the string.
	uint64 _Length;

};