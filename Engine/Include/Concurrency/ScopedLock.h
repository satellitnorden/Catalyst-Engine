#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Preprocess defines.
#define SCOPED_LOCK(LOCK) ScopedLock<decltype(LOCK)> scoped_lock{ LOCK };

template <typename TYPE>
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
	ScopedLock(TYPE &newLock) NOEXCEPT
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
	TYPE &_Lock;

};