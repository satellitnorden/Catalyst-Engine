#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Intrinsics.
#include <immintrin.h>

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
				_mm_pause();
			}

			else
			{
				std::this_thread::yield();
				spin_count = 0;
			}
		}
	}

	/*
	*	Unlocks the lock.
	*/
	FORCE_INLINE void Unlock() NOEXCEPT
	{
		_Locked.store(false, std::memory_order_release);
	}

private:

	//Denotes whether or not this lock is locked.
	std::atomic<bool> _Locked{ false };

	/*
	*	Tries to lock the lock. Returns if locking was successful.
	*/
	FORCE_INLINE NO_DISCARD bool TryLock() NOEXCEPT
	{
		return !_Locked.load(std::memory_order_relaxed) && !_Locked.exchange(true, std::memory_order_acquire);
	}

};