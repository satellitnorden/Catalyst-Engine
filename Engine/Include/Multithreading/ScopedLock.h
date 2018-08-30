#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

template <typename Type>
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
	ScopedLock(Type &newLock) NOEXCEPT
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
	Type &lock;

};