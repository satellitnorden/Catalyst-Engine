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
	*	Pre-updates this updateable synchronously.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool PreUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "PreUpdateSynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Updates this updateable synchronously.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool UpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "UpdateSynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Post-updates this updateable synchronously.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool PostUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "PostUpdateSynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Pre-updates this updateable asynchronously.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool PreUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "PreUpdateAsynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Updates this updateable asynchronously.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool UpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "UpdateAsynchronous called on updateable that has not overriden this function.");

		return false;
	}

	/*
	*	Post-updates this updateable asynchronously.
	*	Should return whether or not this updateable should continue to be updated.
	*/
	virtual bool PostUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
	{
		ASSERT(false, "PostUpdateAsynchronous called on updateable that has not overriden this function.");

		return false;
	}

};