#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class Semaphore final
{

public:

	/*
	*	Default constructor.
	*/
	Semaphore() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the signal count as an argument.
	*/
	Semaphore(const uint64 newSignalCount) NOEXCEPT
	{

	}

	/*
	*	Resets this semaphore.
	*/
	void Reset() NOEXCEPT { signalled.store(false); }

	/*
	*	Signals this semaphore.
	*/
	void Signal() NOEXCEPT { signalled.store(true); }

	/*
	*	Waits for this semaphore.
	*/
	void WaitFor() NOEXCEPT { while (!signalled) THREAD_YIELD(); }

private:

	//Defines whether or not this semaphore is signalled.
	std::atomic<bool> signalled{ false };

};