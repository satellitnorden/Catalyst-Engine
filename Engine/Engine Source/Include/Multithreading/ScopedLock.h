#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

template <class LockType>
class ScopedLock final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	ScopedLock() NOEXCEPT = delete;

	/*
	*	Constructor taking the lock as it's argument.
	*/
	ScopedLock(LockType &newLock) NOEXCEPT
		:
		lock(newLock)
	{
		//Lock the lock.
		lock.Lock();
	}

	/*
	*	Default destructor.
	*/
	~ScopedLock() NOEXCEPT
	{
		//Unlock the lock.
		lock.Unlock();
	}

private:

	//The underlying lock for this scoped lock.
	LockType &lock;

};