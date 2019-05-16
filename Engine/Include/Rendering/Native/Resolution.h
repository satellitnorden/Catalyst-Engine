#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Resolution final
{

public:

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	/*
	*	Default constructor.
	*/
	Resolution() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*/
	Resolution(const uint32 initialWidth, const uint32 initialHeight) NOEXCEPT
		:
		_Width(initialWidth),
		_Height(initialHeight)
	{

	}

	/*
	*	Division operator overload.	
	*/
	Resolution operator/(const uint32 scalar) NOEXCEPT
	{
		return Resolution(_Width / scalar, _Height / scalar);
	}

	/*
	*	Multiplication operator overload.
	*/
	Resolution operator*(const uint32 scalar) NOEXCEPT
	{
		return Resolution(_Width * scalar, _Height * scalar);
	}

};