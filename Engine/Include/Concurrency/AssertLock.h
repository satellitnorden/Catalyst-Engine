#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>

#if !defined(CATALYST_CONFIGURATION_FINAL)
class AssertLock final
{

public:

	/*
	*	Locks this assert lock.
	*/
	FORCE_INLINE void Lock() NOEXCEPT
	{
		//Increment the lock count.
		const uint32 previous_lock_count{ _LockCount.FetchAdd(1) };

		ASSERT(previous_lock_count == 0, "There is contention on this lock!");
	}

	/*
	*	Unlocks the lock.
	*/
	FORCE_INLINE void Unlock() NOEXCEPT
	{
		//Decrement the lock count.
		const uint32 previous_lock_count{ _LockCount.FetchSub(1) };

		ASSERT(previous_lock_count == 1, "There is contention on this lock!");
	}

private:

	//The lock count.
	Atomic<uint32> _LockCount{ 0 };

};
#else
class AssertLock final
{

public:

	/*
	*	Locks this assert lock.
	*/
	FORCE_INLINE constexpr void Lock() NOEXCEPT
	{

	}

	/*
	*	Unlocks the lock.
	*/
	FORCE_INLINE constexpr void Unlock() NOEXCEPT
	{

	}

};
#endif