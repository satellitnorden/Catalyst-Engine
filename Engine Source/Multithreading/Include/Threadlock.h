#pragma once

//Engine core.
#include <EngineCore.h>

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
		static bool expected = false;
		while (!lock.compare_exchange_weak(expected, true))
		{
			THREAD_YIELD();
		}
	}

	/*
	*	Unlocks this threadlock.
	*/
	void Unlock() NOEXCEPT
	{
		lock.store(false);
	}

private:

	//The underlying atomic lock.
	std::atomic<bool> lock{ false };

};
