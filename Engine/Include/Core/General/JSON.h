#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

class JSON final
{

public:

	//Constants.
	constexpr static uint64 ANY_SIZE{ 32 };

	/*
	*	Default constructor.
	*/
	JSON() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~JSON() NOEXCEPT;

	/*
	*	Assignment operator taking a C string.
	*/
	void operator=(const char *const RESTRICT value) NOEXCEPT;

	/*
	*	Assignment operator taking a bool.
	*/
	void operator=(const bool value) NOEXCEPT;

	/*
	*	Assignment operator taking a float64.
	*/
	void operator=(const float64 value) NOEXCEPT;

	/*
	*	Assignment operator taking a uint64.
	*/
	void operator=(const uint64 value) NOEXCEPT;

	/*
	*	Subscript operator overload.
	*/
	JSON operator[](const char *const RESTRICT key) NOEXCEPT;

	/*
	*	Reads from the given file path.
	*/
	void Read(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Writes to the given file path.
	*/
	void Write(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Emplaces a new object under this JSON object.
	*/
	JSON Emplace() NOEXCEPT;

private:

	//The implementation.
	Any<ANY_SIZE> _Implementation;

};