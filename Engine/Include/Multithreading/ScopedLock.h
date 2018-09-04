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
		_Lock(newLock)
	{
		//Lock the lock.
		_Lock.Lock();
	}

	/*
	*	Default destructor.
	*/
	~ScopedLock() NOEXCEPT
	{
		//Unlock the lock.
		_Lock.Unlock();
	}

private:

	//The underlying lock for this scoped lock.
	Type &_Lock;

};