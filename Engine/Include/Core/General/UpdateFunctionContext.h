#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>

class UpdateFunctionContext final
{

public:

	//The update context.
	const UpdateContext *RESTRICT _Context;

	//The arguments.
	const void *RESTRICT _Arguments;

	/*
	*	Default constructor.
	*/
	UpdateFunctionContext() NOEXCEPT { }

	/*
	*	Constructor taking all values as arguments.
	*/
	UpdateFunctionContext(const UpdateContext *const RESTRICT context, const void *const RESTRICT arguments) NOEXCEPT
		:
		_Context(context),
		_Arguments(arguments)
	{

	}

};