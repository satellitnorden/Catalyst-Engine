#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename TYPE>
class ScopedWriteLock final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	ScopedWriteLock() NOEXCEPT = delete;

	/*
	*	Constructor taking the lock as it's argument.
	*/
	ScopedWriteLock(TYPE &newLock) NOEXCEPT
		:
		_Lock(newLock)
	{
		//Lock the lock.
		_Lock.WriteLock();
	}

	/*
	*	Default destructor.
	*/
	~ScopedWriteLock() NOEXCEPT
	{
		//Unlock the lock.
		_Lock.WriteUnlock();
	}

private:

	//The underlying lock for this scoped lock.
	TYPE &_Lock;

};