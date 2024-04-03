#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

class BinaryInputFile final
{
	
public:

	constexpr static uint64 ANY_SIZE{ 80 };

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	BinaryInputFile() NOEXCEPT = delete;

	/*
	*	Constructor taking the file path.
	*/
	BinaryInputFile(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Bool operator overload.
	*/
	NO_DISCARD operator bool() NOEXCEPT;

	/*
	*	Returns the file path.
	*/
	RESTRICTED NO_DISCARD const char *const RESTRICT GetFilePath() const NOEXCEPT;

	/*
	*	Returns the size of this binary input file, in bytes.
	*/
	NO_DISCARD uint64 Size() const NOEXCEPT;

	/*
	*	Reads from this binary file.
	*/
	void Read(void *const RESTRICT output, const uint64 size) NOEXCEPT;

	/*
	*	Returns the current position, in bytes.
	*/
	NO_DISCARD uint64 GetCurrentPosition() NOEXCEPT;

	/*
	*	Sets the current position, in bytes.
	*/
	void SetCurrentPosition(const uint64 position) NOEXCEPT;

	/*
	*	Skips a number of bytes.
	*/
	void Skip(const uint64 size) NOEXCEPT;

	/*
	*	Returns if this binary file has reached the end of file.
	*/
	NO_DISCARD bool HasReachedEndOfFile() NOEXCEPT;

	/*
	*	Closes this binary file.
	*/
	void Close() NOEXCEPT;

private:

	//The implementation.
	Any<ANY_SIZE> _Implementation;

};