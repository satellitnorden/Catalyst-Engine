#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency
#include <Concurrency/Atomic.h>
#include <Concurrency/Concurrency.h>

class AtomicFlag final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AtomicFlag() NOEXCEPT
		:
		_Flag(false)
	{

	}

	/*
	*	Constructor taking the initial state.
	*/
	FORCE_INLINE AtomicFlag(const bool initial_state) NOEXCEPT
		:
		_Flag(initial_state)
	{

	}

	/*
	*	Sets this atomic flag.
	*/
	FORCE_INLINE void Set() NOEXCEPT
	{
		_Flag.Store(true);
	}

	/*
	*	Clears this atomic flag.
	*/
	FORCE_INLINE void Clear() NOEXCEPT
	{
		_Flag.Store(false);
	}

	/*
	*	Returns if this atomic flag is set.
	*/
	FORCE_INLINE NO_DISCARD bool IsSet() const NOEXCEPT
	{
		return _Flag.Load();
	}

	/*
	*	Waits for this atomic flag to be set.
	*/
	template <WaitMode MODE>
	FORCE_INLINE void Wait() const NOEXCEPT
	{
		if constexpr (MODE == WaitMode::PAUSE)
		{
			while (!IsSet())
			{
				Concurrency::CurrentThread::Pause();
			}
		}

		else if constexpr (MODE == WaitMode::YIELD)
		{
			while (!IsSet())
			{
				Concurrency::CurrentThread::Yield();
			}
		}

		else
		{
			ASSERT(false, "Invalid wait mode!");
		}
	}

private:

	//The underlying atomic flag.
	Atomic<bool> _Flag;

};