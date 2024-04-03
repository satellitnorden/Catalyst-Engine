#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

class BinaryOutputFile final
{
	
public:

	constexpr static uint64 ANY_SIZE{ 272 };

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	BinaryOutputFile() NOEXCEPT = delete;

	/*
	*	Constructor taking the file path.
	*/
	BinaryOutputFile(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Returns the file path.
	*/
	RESTRICTED NO_DISCARD const char *const RESTRICT GetFilePath() const NOEXCEPT;

	/*
	*	Writes to this file.
	*/
	void Write(const void *const RESTRICT input, const uint64 size) NOEXCEPT;

	/*
	*	Closes this file.
	*/
	void Close() NOEXCEPT;

private:

	//The implementation.
	Any<ANY_SIZE> _Implementation;

};