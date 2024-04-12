#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

class JSON final
{

public:

	//Constants.
	constexpr static uint64 ANY_SIZE{ 16 };

	/*
	*	Default constructor.
	*/
	JSON() NOEXCEPT;

	/*
	*	Constructor taking a file path.
	*/
	JSON(const char *const RESTRICT file_path) NOEXCEPT;

	/*
	*	Subscript operator overload.
	*/
	JSON operator[](const char *const RESTRICT key) NOEXCEPT;

private:

	//The implementation.
	Any<ANY_SIZE> _Implementation;

};