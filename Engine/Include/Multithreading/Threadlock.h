#pragma once

//Core.
#include <Core/General/EngineCore.h>

class Threadlock final
{

public:

	/*
	*	Default constructor.
	*/
	Threadlock() NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~Threadlock() NOEXCEPT
	{

	}

	/*
	*	Locks this threadlock.
	*/
	void Lock() NOEXCEPT
	{
		while (lock.test_and_set(std::memory_order_acquire))
		{
			std::this_thread::yield();
		}
	}

	/*
	*	Unlocks this threadlock.
	*/
	void Unlock() NOEXCEPT
	{
		lock.clear(std::memory_order_release);
	}

private:

	//The underlying atomic lock.
	std::atomic_flag lock{ ATOMIC_FLAG_INIT };

};
