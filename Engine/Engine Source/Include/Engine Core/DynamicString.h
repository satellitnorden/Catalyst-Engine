#pragma once

class DynamicString final
{

public:

	/*
	*	Default constructor.
	*/
	DynamicString() NOEXCEPT
		:
		string(nullptr),
		length(0)
	{

	}

	/*
	*	Copy constructor.
	*/
	DynamicString(const DynamicString &otherString) NOEXCEPT
	{
		//Set the length of this string.
		length = otherString.Length();

		//Allocate sufficient memory for the underlying string.
		string = StaticCast<char *RESTRICT>(MemoryUtilities::AllocateMemory(length + 1));

		//Copy the contents of the other string.
		MemoryUtilities::CopyMemory(string, otherString.string, length + 1);
	}

	/*
	*	Move constructor.
	*/
	DynamicString(DynamicString &&otherString) NOEXCEPT
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
	DynamicString(const char *const RESTRICT newString)
	{
		//Determine how long the C string is.
		length = strlen(newString);

		//Allocate sufficient memory to host the string.
		string = StaticCast<char *RESTRICT>(MemoryUtilities::AllocateMemory(length + 1));

		//Copy the string to the memory.
		MemoryUtilities::CopyMemory(StaticCast<void *RESTRICT>(string), StaticCast<const void *const RESTRICT>(newString), length + 1);
	}

	/*
	*	Default destructor.
	*/
	~DynamicString() NOEXCEPT
	{
		//Free the underlying string.
		MemoryUtilities::FreeMemory(string);
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const DynamicString &otherString) NOEXCEPT
	{
		//Update the length of this string.
		length = otherString.Length();

		//Reallocate sufficient memory for the underlying string.
		string = StaticCast<char *RESTRICT>(MemoryUtilities::ReallocateMemory(StaticCast<void *RESTRICT>(string), length + 1));

		//Copy the contents of the other string.
		MemoryUtilities::CopyMemory(string, otherString.string, length + 1);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(DynamicString &&otherString) NOEXCEPT
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
	void operator+=(const char *const RESTRICT newString) NOEXCEPT
	{
		//Calculate the new length.
		const uint64 newStringLength = strlen(newString) + 1;
		const uint64 newLength = length + newStringLength - 1;

		//Allocate sufficient memory to host the concatenated string.
		string = StaticCast<char *RESTRICT>(MemoryUtilities::ReallocateMemory(StaticCast<void *RESTRICT>(string), newLength + 1));

		//Copy the new string.
		MemoryUtilities::CopyMemory(string + length, newString, newStringLength);

		//Update the length of the string.
		length = newLength;
	}

	/*
	*	Less than operator overload.
	*/
	bool operator<(const DynamicString &otherString) const NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns the underlying C string.
	*/
	RESTRICTED const char *const CString() const NOEXCEPT
	{
		return string;
	}

	/*
	*	Returns the length of the string.
	*/
	uint64 Length() const NOEXCEPT
	{
		return length;
	}

private:

	//The underlying C string.
	char *RESTRICT string;

	//The length of the string.
	uint64 length;

};