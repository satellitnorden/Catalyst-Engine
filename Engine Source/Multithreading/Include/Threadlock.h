#pragma once

//Engine core.
#include <EngineCore.h>

class Threadlock final
{

public:

	/*
	*	Default constructor.
	*/
	Threadlock() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~Threadlock() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Locks this threadlock.
	*/
	void Lock() CATALYST_NOEXCEPT
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
	void Unlock() CATALYST_NOEXCEPT
	{
		lock.store(false);
	}

private:

	//The underlying atomic lock.
	std::atomic<bool> lock{ false };

};
