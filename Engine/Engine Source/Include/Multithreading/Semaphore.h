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
		:
		signalCount(newSignalCount)
	{

	}

	/*
	*	Initializes the signal count of this semaphore.
	*/
	void InitializeSignalCount(const uint64 newSignalCount) NOEXCEPT
	{
		signalCount = newSignalCount;
	}

	/*
	*	Resets this semaphore.
	*/
	void Reset() NOEXCEPT { signalCounter.store(0); }

	/*
	*	Signals this semaphore.
	*/
	void Signal() NOEXCEPT { ++signalCounter; }

	/*
	*	Waits for this semaphore.
	*/
	void WaitFor() NOEXCEPT { while (signalCounter < signalCount) THREAD_YIELD(); }

private:

	//The signal counter.
	std::atomic<uint64> signalCounter{ 0 };

	//The signal count.
	uint64 signalCount{ 0 };

};