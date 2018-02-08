#pragma once

//Engine core.
#include <EngineCore.h>

class Semaphore final
{

public:

	/*
	*	Default constructor.
	*/
	Semaphore() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Constructor taking the signal count as an argument.
	*/
	Semaphore(const uint64 newSignalCount) CATALYST_NOEXCEPT
		:
		signalCount(newSignalCount)
	{

	}

	/*
	*	Initializes the signal count of this semaphore.
	*/
	void InitializeSignalCount(const uint64 newSignalCount) CATALYST_NOEXCEPT
	{
		signalCount = newSignalCount;
	}

	/*
	*	Resets this semaphore.
	*/
	void Reset() CATALYST_NOEXCEPT { signalCounter.store(0); }

	/*
	*	Signals this semaphore.
	*/
	void Signal() CATALYST_NOEXCEPT { ++signalCounter; }

	/*
	*	Waits for this semaphore.
	*/
	void WaitFor() CATALYST_NOEXCEPT { while (signalCounter < signalCount) THREAD_YIELD(); }

private:

	//The signal counter.
	std::atomic<uint64> signalCounter{ 0 };

	//The signal count.
	uint64 signalCount{ 0 };

};