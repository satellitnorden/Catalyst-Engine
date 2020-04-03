#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/ConcurrencyCore.h>

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
	*	Copy assignment operator overload.
	*/
	void operator=(const Semaphore &other) NOEXCEPT
	{
		_Signalled.store(other._Signalled.load());
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
			Concurrency::CurrentThread::Yield();
		}
	}

private:

	//Defines whether or not this semaphore is signalled.
	Atomic<bool> _Signalled{ false };

};