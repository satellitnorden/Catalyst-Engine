#pragma once

//Engine core.
#include <EngineCore.h>

template <class LockType>
class ScopedLock
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	ScopedLock() CATALYST_NOEXCEPT = delete;

	/*
	*	Constructor taking the lock as it's argument.
	*/
	ScopedLock(LockType &newLock) CATALYST_NOEXCEPT
		:
	lock(newLock)
	{
		//Lock the lock.
		lock.Lock();
	}

	/*
	*	Default destructor.
	*/
	~ScopedLock() CATALYST_NOEXCEPT
	{
		//Unlock the lock.
		lock.Unlock();
	}

private:

	//The underlying lock for this scoped lock.
	LockType &lock;

};