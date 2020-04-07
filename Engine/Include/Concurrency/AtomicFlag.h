#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/ConcurrencyCore.h>

class AtomicFlag final
{

public:

	/*
	*	Sets this atomic flag.
	*/
	FORCE_INLINE void Set() NOEXCEPT
	{
		_Flag.store(true, std::memory_order_release);
	}

	/*
	*	Clears this atomic flag.
	*/
	FORCE_INLINE void Clear() NOEXCEPT
	{
		_Flag.store(false, std::memory_order_release);
	}

	/*
	*	Returns if this atomic flag is set.
	*/
	FORCE_INLINE NO_DISCARD bool IsSet() const NOEXCEPT
	{
		return _Flag.load(std::memory_order_acquire);
	}

	/*
	*	Waits for this atomic flag to be set.
	*/
	template <WaitMode MODE>
	FORCE_INLINE void Wait() const NOEXCEPT
	{
		ASSERT(false, "Not implemented!");
	}

	/*
	*	Waits for this atomic flag to be set.
	*/
	template <>
	FORCE_INLINE void Wait<WaitMode::PAUSE>() const NOEXCEPT
	{
		while (!IsSet())
		{
			Concurrency::CurrentThread::Pause();
		}
	}

	/*
	*	Waits for this atomic flag to be set.
	*/
	template <>
	FORCE_INLINE void Wait<WaitMode::YIELD>() const NOEXCEPT
	{
		while (!IsSet())
		{
			Concurrency::CurrentThread::Yield();
		}
	}

private:

	//The underlying atomic flag.
	Atomic<bool> _Flag{ false };

};

static_assert(Atomic<bool>::is_always_lock_free, "Atomic bools are assumed to be lock-free!");