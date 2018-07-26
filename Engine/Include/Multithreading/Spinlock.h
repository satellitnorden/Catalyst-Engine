#pragma once

//Core.
#include <Core/EngineCore.h>

class Spinlock final
{

public:

	/*
	*	Default constructor.
	*/
	Spinlock() NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~Spinlock() NOEXCEPT
	{

	}

	/*
	*	Locks this spinlock.
	*/
	void Lock() NOEXCEPT
	{
		while (!lock.test_and_set(std::memory_order_acquire));
	}

	/*
	*	Unlocks this spinlock.
	*/
	void Unlock() NOEXCEPT
	{
		lock.clear(std::memory_order_release);
	}

private:

	//The underlying atomic lock.
	std::atomic_flag lock{ ATOMIC_FLAG_INIT };

};