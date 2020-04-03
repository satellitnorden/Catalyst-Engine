#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Preprocess defines.
#define SCOPED_READ_LOCK(LOCK) ScopedReadLock<decltype(LOCK)> scoped_lock{ LOCK };

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