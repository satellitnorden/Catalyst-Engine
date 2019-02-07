#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

#if true
class Spinlock final
{

public:

	/*
	*	Locks this spinlock for a read operation.
	*/
	void ReadLock() NOEXCEPT
	{
		for (;;)
		{
			//Wait for write to end.
			while (_WriteInProgress.load(std::memory_order_consume));

			//Increase the number of readers.
			_NumberOfReaders.fetch_add(1, std::memory_order_acquire);

			//If a write began during the transaction, decrease the number of readers and try again.
			if (_WriteInProgress.load(std::memory_order_consume))
			{
				_NumberOfReaders.fetch_sub(1, std::memory_order_release);
			}

			else
			{
				return;
			}
		}
	}

	/*
	*	Unlocks this spinlock for a read operation.
	*/
	void ReadUnlock() NOEXCEPT
	{
		//Decrease the number of readers.
		_NumberOfReaders.fetch_sub(1, std::memory_order_release);
	}

	/*
	*	Locks this spinlock for a write operation.
	*/
	void WriteLock() NOEXCEPT
	{
		//Acquire the lock.
		bool expected{ false };
		while (!_WriteInProgress.compare_exchange_weak(expected, true, std::memory_order_acquire))
		{
			expected = false;
		}

		//Wait for the number of readers to decrease to zero before writing.
		while (_NumberOfReaders.load(std::memory_order_consume) > 0);
	}

	/*
	*	Unlocks this spinlock for a write operation.
	*/
	void WriteUnlock() NOEXCEPT
	{
		//Release the lock.
		_WriteInProgress.exchange(false, std::memory_order_release);
	}

private:

	//The number of readers.
	std::atomic<uint64> _NumberOfReaders{ 0 };

	//Denotes whether or not a write is in progress.
	std::atomic<bool> _WriteInProgress{ false };

};
#else
class Spinlock final
{

public:

	/*
	*	Locks this spinlock for a read operation.
	*/
	void ReadLock() NOEXCEPT
	{
		while (_Lock.test_and_set(std::memory_order_acquire));
	}

	/*
	*	Unlocks this spinlock for a read operation.
	*/
	void ReadUnlock() NOEXCEPT
	{
		_Lock.clear(std::memory_order_release);
	}

	/*
	*	Locks this spinlock for a write operation.
	*/
	void WriteLock() NOEXCEPT
	{
		while (_Lock.test_and_set(std::memory_order_acquire));
	}

	/*
	*	Unlocks this spinlock for a write operation.
	*/
	void WriteUnlock() NOEXCEPT
	{
		_Lock.clear(std::memory_order_release);
	}

private:

	//The underlying atomic lock.
	std::atomic_flag _Lock{ ATOMIC_FLAG_INIT };

};
#endif