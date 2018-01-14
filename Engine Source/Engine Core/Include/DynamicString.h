#pragma once

class DynamicString
{

public:

	/*
	*	Default constructor.
	*/
	DynamicString() CATALYST_NOEXCEPT
		:
		string(nullptr),
		length(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	DynamicString(const DynamicString &otherString) CATALYST_NOEXCEPT
	{
		//Set the length of this string.
		length = otherString.Length();

		//Allocate sufficient memory for the underlying string.
		string = static_cast<char *CATALYST_RESTRICT>(MemoryUtilities::AllocateMemory(length + 1));

		//Copy the contents of the other string.
		MemoryUtilities::CopyMemory(string, otherString.string, length + 1);
	}

	/*
	*	Move constructor.
	*/
	DynamicString(DynamicString &&otherString) CATALYST_NOEXCEPT
	{
		//Steal the other string's underlying string and set it to null!
		string = otherString.string;
		otherString.string = nullptr;

		//Steal the other string's length!
		length = otherString.length;
	}

	/*
	*	Constructor taking a C string.
	*/
	DynamicString(const char *const CATALYST_RESTRICT newString)
	{
		//Determine how long the C string is.
		length = strlen(newString);

		//Allocate sufficient memory to host the string.
		string = static_cast<char *CATALYST_RESTRICT>(MemoryUtilities::AllocateMemory(length + 1));

		//Copy the string to the memory.
		MemoryUtilities::CopyMemory(static_cast<void *CATALYST_RESTRICT>(string), static_cast<const void *const CATALYST_RESTRICT>(newString), length + 1);
	}

	/*
	*	Default destructor.
	*/
	~DynamicString() CATALYST_NOEXCEPT
	{
		//Free the underlying string.
		MemoryUtilities::FreeMemory(string);
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const DynamicString &otherString) CATALYST_NOEXCEPT
	{
		//Update the length of this string.
		length = otherString.Length();

		//Reallocate sufficient memory for the underlying string.
		string = static_cast<char *CATALYST_RESTRICT>(MemoryUtilities::ReallocateMemory(static_cast<void *CATALYST_RESTRICT>(string), length + 1));

		//Copy the contents of the other string.
		MemoryUtilities::CopyMemory(string, otherString.string, length + 1);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(DynamicString &&otherString) CATALYST_NOEXCEPT
	{
		//Steal the other string's underlying string and set it to nullptr!
		string = otherString.string;
		otherString.string = nullptr;

		//Steal the other string's length!
		length = otherString.length;
	}

	/*
	*	Addition assignment operator overload.
	*/
	void operator+=(const char *const CATALYST_RESTRICT newString) CATALYST_NOEXCEPT
	{
		//Calculate the new length.
		const size_t newStringLength = strlen(newString) + 1;
		const size_t newLength = length + newStringLength - 1;

		//Allocate sufficient memory to host the concatenated string.
		string = static_cast<char *CATALYST_RESTRICT>(MemoryUtilities::ReallocateMemory(static_cast<void *CATALYST_RESTRICT>(string), newLength + 1));

		//Copy the new string.
		MemoryUtilities::CopyMemory(string + length, newString, newStringLength);

		//Update the length of the string.
		length = newLength;
	}

	/*
	*	Less than operator overload.
	*/
	bool operator<(const DynamicString &otherString) const CATALYST_NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns the underlying C string.
	*/
	CATALYST_RESTRICTED const char *const CString() const CATALYST_NOEXCEPT
	{
		return string;
	}

	/*
	*	Returns the length of the string.
	*/
	size_t Length() const CATALYST_NOEXCEPT
	{
		return length;
	}

private:

	//The underlying C string.
	char *CATALYST_RESTRICT string;

	//The length of the string.
	size_t length;

};