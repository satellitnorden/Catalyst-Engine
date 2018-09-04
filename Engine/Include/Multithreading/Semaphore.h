#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Enumeration covering all semaphore creation flags.
enum SemaphoreCreationFlags : uint8
{
	Unsignalled = BIT(0),
	Signalled = BIT(1)
};

class Semaphore final
{

public:

	/*
	*	Default constructor.
	*/
	Semaphore(const SemaphoreCreationFlags creationFlags = SemaphoreCreationFlags::Unsignalled) NOEXCEPT
		:
		_Signalled(creationFlags & SemaphoreCreationFlags::Signalled)
	{

	}

	/*
	*	Signals this semaphore.
	*/
	void Signal() NOEXCEPT { _Signalled.store(true); }

	/*
	*	Resets this semaphore.
	*/
	void Reset() NOEXCEPT { _Signalled.store(false); }

	/*
	*	Returns whether or not this semaphore is signalled.
	*/
	bool IsSignalled() const NOEXCEPT { return _Signalled.load(); }

	/*
	*	Waits for this semaphore.
	*/
	void WaitFor() const NOEXCEPT
	{
		while (!_Signalled)
		{
			std::this_thread::yield();
		}
	}

private:

	//Defines whether or not this semaphore is signalled.
	std::atomic<bool> _Signalled{ false };

};