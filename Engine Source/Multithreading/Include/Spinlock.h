#pragma once

//Engine core.
#include <EngineCore.h>

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
		static bool expected = false;
		while (!lock.compare_exchange_weak(expected, true));
	}

	/*
	*	Unlocks this spinlock.
	*/
	void Unlock() NOEXCEPT
	{
		lock.store(false);
	}

private:

	//The underlying atomic lock.
	std::atomic<bool> lock{ false };

};