#pragma once

//Engine core.
#include <EngineCore.h>

class Spinlock
{

public:

	/*
	*	Default constructor.
	*/
	Spinlock() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~Spinlock() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Locks this spinlock.
	*/
	void Lock() CATALYST_NOEXCEPT
	{
		static bool expected = false;
		while (!lock.compare_exchange_weak(expected, true));
	}

	/*
	*	Unlocks this spinlock.
	*/
	void Unlock() CATALYST_NOEXCEPT
	{
		lock.store(false);
	}

private:

	//The underlying atomic lock.
	std::atomic<bool> lock{ false };

};