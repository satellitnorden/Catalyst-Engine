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
	*	Updates this updateable synchronously during the opening update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool OpeningUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "OpeningUpdateSynchronous called on updateable that has not overriden this function.");

		return false;
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
	*	Updates this updateable synchronously during the opening render update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool RenderUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "RenderUpdateSynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Updates this updateable synchronously during the opening closing update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool ClosingUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "ClosingUpdateSynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Updates this updateable asynchronously during the opening update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool OpeningUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "OpeningUpdateAsynchronous called on updateable that has not overriden this function.");

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

	/*
	*	Updates this updateable asynchronously during the render update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool RenderUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "RenderUpdateAsynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Updates this updateable asynchronously during the closing update phase.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool ClosingUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "ClosingUpdateAsynchronous called on updateable that has not overriden this function.");

		return false;
	}

};