#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>

class Updateable
{

public:

	/*
	*	Default constructor.
	*/
	Updateable() NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	virtual ~Updateable() NOEXCEPT
	{

	}

	/*
	*	Updates this updateable synchronously during the logic update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool LogicUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "LogicUpdateSynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Updates this updateable asynchronously during the logic update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "LogicUpdateAsynchronous called on updateable that has not overriden this function.");

		return false;
	}

};