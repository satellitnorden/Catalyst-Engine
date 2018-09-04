#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class Threadlock final
{

public:

	/*
	*	Locks this threadlock.
	*/
	void Lock() NOEXCEPT
	{
		while (_Lock.test_and_set(std::memory_order_acquire))
		{
			std::this_thread::yield();
		}
	}

	/*
	*	Unlocks this threadlock.
	*/
	void Unlock() NOEXCEPT
	{
		_Lock.clear(std::memory_order_release);
	}

private:

	//The underlying atomic lock.
	std::atomic_flag _Lock{ ATOMIC_FLAG_INIT };

};
