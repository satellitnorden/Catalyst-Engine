#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class Resolution final
{

public:

	//The width.
	uint32 width;

	//The height.
	uint32 height;

	/*
	*	Default constructor.
	*/
	Resolution() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*/
	Resolution(const uint32 initialWidth, const uint32 initialHeight) NOEXCEPT
		:
		width(initialWidth),
		height(initialHeight)
	{

	}

	/*
	*	Division operator overload.	
	*/
	Resolution operator/(const uint32 scalar) NOEXCEPT
	{
		return Resolution(width / scalar, height / scalar);
	}

};