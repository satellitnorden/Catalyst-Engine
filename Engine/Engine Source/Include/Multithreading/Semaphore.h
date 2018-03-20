#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Enumeration covering all semaphore creation flags.
enum SemaphoreCreationFlags : uint8
{
	Unsignalled = 1 << 0,
	Signalled = 1 << 1
};

class Semaphore final
{

public:

	/*
	*	Default constructor.
	*/
	Semaphore(const SemaphoreCreationFlags creationFlags = SemaphoreCreationFlags::Unsignalled) NOEXCEPT
		:
		signalled(creationFlags & SemaphoreCreationFlags::Signalled)
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
	void WaitFor() const NOEXCEPT { while (!signalled) THREAD_YIELD(); }

private:

	//Defines whether or not this semaphore is signalled.
	std::atomic<bool> signalled{ false };

};