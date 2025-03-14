#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/Concurrency.h>

#if defined(CATALYST_CXX20) && 0 //C++20 implementation of a spinlock. NOTE - Doesn't work I guess for C++20. Just go with old version. :x
class Spinlock final
{

public:

	/*
	*	Locks this spinlock.
	*/
	FORCE_INLINE void Lock() NOEXCEPT
	{
		while (_Locked.test_and_set(std::memory_order_acquire))
		{
			_Locked.wait(true, std::memory_order_relaxed);
		}
	}

	/*
	*	Unlocks the lock.
	*/
	FORCE_INLINE void Unlock() NOEXCEPT
	{
		_Locked.clear(false, std::memory_order_release);
		_Locked.notify_one();
	}

private:

	//Denotes whether or not this lock is locked.
	std::atomic_flag _Locked{ ATOMIC_FLAG_INIT };

};
#else //Normal implementation of a spinlock.
class Spinlock final
{

public:

	/*
	*	Locks this spinlock.
	*/
	FORCE_INLINE void Lock() NOEXCEPT
	{
		//Define constants.
		constexpr uint8 MAXIMUM_SPIN_COUNT{ 16 };

		for (uint8 spin_count{ 0 }; !TryLock(); ++spin_count)
		{
			if (spin_count < MAXIMUM_SPIN_COUNT)
			{
				Concurrency::CurrentThread::Pause();
			}

			else
			{
				//Yield the current thread.
				Concurrency::CurrentThread::Yield();

				//Reset the spin count.
				spin_count = 0;
			}
		}
	}

	/*
	*	Unlocks the lock.
	*/
	FORCE_INLINE void Unlock() NOEXCEPT
	{
		_Locked.Store(false);
	}

private:

	//Denotes whether or not this lock is locked.
	Atomic<bool> _Locked{ false };

	/*
	*	Tries to lock the lock. Returns if locking was successful.
	*/
	FORCE_INLINE NO_DISCARD bool TryLock() NOEXCEPT
	{
		return !_Locked.Load() && !_Locked.Exchange(true);
	}

};
#endif