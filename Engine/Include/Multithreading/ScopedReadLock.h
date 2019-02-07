#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

template <typename TYPE>
class ScopedReadLock final
{

public:

	/*
	*	Default constructor - prohibited, must be constructed with the proper arguments.
	*/
	ScopedReadLock() NOEXCEPT = delete;

	/*
	*	Constructor taking the lock as it's argument.
	*/
	ScopedReadLock(TYPE &newLock) NOEXCEPT
		:
		_Lock(newLock)
	{
		//Lock the lock.
		_Lock.ReadLock();
	}

	/*
	*	Default destructor.
	*/
	~ScopedReadLock() NOEXCEPT
	{
		//Unlock the lock.
		_Lock.ReadUnlock();
	}

private:

	//The underlying lock for this scoped lock.
	TYPE &_Lock;

};