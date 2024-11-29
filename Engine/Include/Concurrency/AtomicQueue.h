#pragma once

//Constants.
#define SAFE_ATOMIC_QUEUE (1) //There seems to be some problems with the default atomic queue, so this implements a slower, but more threadsafe atomic queue.

#if SAFE_ATOMIC_QUEUE
//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Optional.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/Spinlock.h>

//Enumeration for all atomic queue modes.
enum class AtomicQueueMode : uint8
{
	SINGLE,
	MULTIPLE
};

/*
*	Varied producer - varied consumers queue - there are overloads for all combinations.
*	WILL overwrite values once the circular buffer has reached the beginning again.
*	This is to avoid reallocations when pushing/popping, so the atomic queue MUST be initialized with large enough storage to ensure that this never happens.
*
*	TYPE should preferably be a small/builtin type that is easily copied.
*	SIZE MUST be a power of two.
*/
template <typename TYPE, uint64 SIZE, AtomicQueueMode PRODUCER_MODE, AtomicQueueMode CONSUMER_MODE>
class AtomicQueue final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AtomicQueue() NOEXCEPT
	{
		//Resize the queue.
		_Queue.Resize<true>(SIZE);
	}

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		//Lock the queue.
		SCOPED_LOCK(_Lock);

		//Add the item.
		_Queue[_LastIndex] = new_value;

		//Increment the last index.
		_LastIndex = (_LastIndex + 1) % _Queue.Size();

		//Increment the number of items in the queue.
		++_NumberOfItemsInQueue;

		ASSERT(_NumberOfItemsInQueue <= SIZE, "Overflow!");
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. 
	*/
	FORCE_INLINE NO_DISCARD Optional<TYPE> Pop() NOEXCEPT
	{
		//Lock the queue.
		SCOPED_LOCK(_Lock);

		//If the first and last index is the same, return nullptr.
		if (_FirstIndex == _LastIndex)
		{
			return Optional<TYPE>();
		}

		//Otherwise, retrieve the correct item.
		const uint64 item_index{ _FirstIndex };

		_FirstIndex = (_FirstIndex + 1) % _Queue.Size();

		--_NumberOfItemsInQueue;

		return Optional<TYPE>(_Queue[item_index]);
	}

private:

	//The lock.
	Spinlock _Lock;

	//The underlying queue.
	DynamicArray<TYPE> _Queue;

	//The first index.
	uint64 _FirstIndex{ 0 };

	//The last index.
	uint64 _LastIndex{ 0 };

	//The number of items in the queue.
	uint64 _NumberOfItemsInQueue{ 0 };

};
#else
//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Concurrency.
#include <Concurrency/Atomic.h>

//Math.
#include <Math/Core/BaseMath.h>

#if defined(CATALYST_CONFIGURATION_FINAL)
	#define VALIDATE_NUMBER_OF_QUEUED_ITEMS (0)
#else
	#define VALIDATE_NUMBER_OF_QUEUED_ITEMS (1)
#endif

//Enumeration for all atomic queue modes.
enum class AtomicQueueMode : uint8
{
	SINGLE,
	MULTIPLE
};

/*
*	Varied producer - varied consumers queue - there are overloads for all combinations.
*	WILL overwrite values once the circular buffer has reached the beginning again.
*	This is to avoid reallocations when pushing/popping, so the atomic queue MUST be initialized with large enough storage to ensure that this never happens.
*	
*	TYPE should preferably be a small/builtin type that is easily copied.
*	SIZE MUST be a power of two.
*/
template <typename TYPE, uint64 SIZE, AtomicQueueMode PRODUCER_MODE, AtomicQueueMode CONSUMER_MODE>
class AtomicQueue final
{

};

/*
*	Specialization for single producer/single consumer.
*/
template<typename TYPE, uint64 SIZE>
class AtomicQueue<TYPE, SIZE, AtomicQueueMode::SINGLE, AtomicQueueMode::SINGLE> final
{

	static_assert(BaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		//Cache the current last index.
		const uint64 current_last_index{ _LastIndex.Load() };

		//Write the new value into the last index.
		_Queue[current_last_index] = new_value;

		//Increment the last index.
		_LastIndex.Store((current_last_index + 1) & (SIZE - 1));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = _NumberOfQueuedItems.Load();
			new_number_of_queued_items = old_number_of_queued_items + 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Pop() NOEXCEPT
	{
		//Cache the current first and last index.
		const uint64 current_first_index{ _FirstIndex.Load() };
		const uint64 current_last_index{ _LastIndex.Load() };

		//If the first and last index are the same, the queue is empty.
		if (current_first_index == current_last_index)
		{
			return nullptr;
		}

		//Cache the value at the current first index.
		TYPE* const RESTRICT current_first_value{ &_Queue[current_first_index] };

		//Increment the first index.
		_FirstIndex.Store((current_first_index + 1) & (SIZE - 1));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = BaseMath::Maximum<uint64>(_NumberOfQueuedItems.Load(), 1);
			new_number_of_queued_items = old_number_of_queued_items - 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif

		//Return the value.
		return current_first_value;
	}

private:

	//The underlying queue.
	StaticArray<TYPE, SIZE> _Queue;

	//The first index in the queue.
	Atomic<uint64> _FirstIndex{ 0 };

	//The last index in the queue.
	Atomic<uint64> _LastIndex{ 0 };

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
	//The number of queued items.
	Atomic<uint64> _NumberOfQueuedItems{ 0 };
#endif

};

/*
*	Specialization for single producer/multiple consumers.
*/
template<typename TYPE, uint64 SIZE>
class AtomicQueue<TYPE, SIZE, AtomicQueueMode::SINGLE, AtomicQueueMode::MULTIPLE> final
{

	static_assert(BaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		//Cache the current last index.
		const uint64 current_last_index{ _LastIndex.Load() };

		//Write the new value into the last index.
		_Queue[current_last_index] = new_value;

		//Increment the last index.
		_LastIndex.Store((current_last_index + 1) & (SIZE - 1));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = _NumberOfQueuedItems.Load();
			new_number_of_queued_items = old_number_of_queued_items + 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Pop() NOEXCEPT
	{
		uint64 old_first_index;
		uint64 old_last_index;
		uint64 new_first_index;

		do
		{
			old_first_index = _FirstIndex.Load();
			old_last_index = _LastIndex.Load();

			if (old_first_index == old_last_index)
			{
				return nullptr;
			}

			new_first_index = old_first_index < (SIZE - 1) ? old_first_index + 1 : 0;
		} while (!_FirstIndex.CompareExchangeWeak(old_first_index, new_first_index));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = BaseMath::Maximum<uint64>(_NumberOfQueuedItems.Load(), 1);
			new_number_of_queued_items = old_number_of_queued_items - 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif

		return &_Queue[old_first_index];
	}

private:

	//The underlying queue.
	StaticArray<TYPE, SIZE> _Queue;

	//The first index in the queue.
	Atomic<uint64> _FirstIndex{ 0 };

	//The last index in the queue.
	Atomic<uint64> _LastIndex{ 0 };

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
	//The number of queued items.
	Atomic<uint64> _NumberOfQueuedItems{ 0 };
#endif

};

/*
*	Specialization for multiple producers/single consumer.
*/
template<typename TYPE, uint64 SIZE>
class AtomicQueue<TYPE, SIZE, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> final
{

	static_assert(BaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		uint64 old_write_index;
		uint64 new_write_index;

		do
		{
			old_write_index = _WriteIndex.Load();

			new_write_index = (old_write_index + 1) & (SIZE - 1);
		} while (!_WriteIndex.CompareExchangeWeak(old_write_index, new_write_index));

		_Queue[old_write_index] = new_value;

		while (!_LastIndex.CompareExchangeWeak(old_write_index, new_write_index));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = _NumberOfQueuedItems.Load();
			new_number_of_queued_items = old_number_of_queued_items + 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Pop() NOEXCEPT
	{
		//Cache the current first and last index.
		const uint64 current_first_index{ _FirstIndex.Load() };
		const uint64 current_last_index{ _LastIndex.Load() };

		//If the first and last index are the same, the queue is empty.
		if (current_first_index == current_last_index)
		{
			return nullptr;
		}

		//Cache the value at the current first index.
		TYPE* const RESTRICT current_first_value{ &_Queue[current_first_index] };

		//Increment the first index.
		_FirstIndex.Store((current_first_index + 1) & (SIZE - 1));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = BaseMath::Maximum<uint64>(_NumberOfQueuedItems.Load(), 1);
			new_number_of_queued_items = old_number_of_queued_items - 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif

		//Return the value.
		return current_first_value;
	}

private:

	//The underlying queue.
	StaticArray<TYPE, SIZE> _Queue;

	//The first index in the queue.
	Atomic<uint64> _FirstIndex{ 0 };

	//The current write index.
	Atomic<uint64> _WriteIndex{ 0 };

	//The last index in the queue.
	Atomic<uint64> _LastIndex{ 0 };

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
	//The number of queued items.
	Atomic<uint64> _NumberOfQueuedItems{ 0 };
#endif

};

/*
*	Specialization for multiple producers/multiple consumers.
*/
template<typename TYPE, uint64 SIZE>
class AtomicQueue<TYPE, SIZE, AtomicQueueMode::MULTIPLE, AtomicQueueMode::MULTIPLE> final
{

	static_assert(BaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		uint64 old_write_index;
		uint64 new_write_index;

		do
		{
			old_write_index = _WriteIndex.Load();
			new_write_index = (old_write_index + 1) & (SIZE - 1);
		} while (!_WriteIndex.CompareExchangeWeak(old_write_index, new_write_index));

		_Queue[old_write_index] = new_value;

		while (!_LastIndex.CompareExchangeWeak(old_write_index, new_write_index));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = _NumberOfQueuedItems.Load();
			new_number_of_queued_items = old_number_of_queued_items + 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Pop() NOEXCEPT
	{
		uint64 old_first_index;
		uint64 old_last_index;
		uint64 new_first_index;

		do
		{
			old_first_index = _FirstIndex.Load();
			old_last_index = _LastIndex.Load();

			if (old_first_index == old_last_index)
			{
				return nullptr;
			}

			new_first_index = (old_first_index + 1) & (SIZE - 1);
		} while (!_FirstIndex.CompareExchangeWeak(old_first_index, new_first_index));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		uint64 old_number_of_queued_items;
		uint64 new_number_of_queued_items;

		do
		{
			old_number_of_queued_items = BaseMath::Maximum<uint64>(_NumberOfQueuedItems.Load(), 1);
			new_number_of_queued_items = old_number_of_queued_items - 1;
		} while (!_NumberOfQueuedItems.CompareExchangeWeak(old_number_of_queued_items, new_number_of_queued_items));

		ASSERT(_NumberOfQueuedItems.Load() < SIZE, "Number of queued items is too high! Number of queued items: %llu, SIZE: %llu.", _NumberOfQueuedItems.Load(), SIZE);
#endif

		return &_Queue[old_first_index];
	}

private:

	//The underlying queue.
	StaticArray<TYPE, SIZE> _Queue;

	//The first index in the queue.
	Atomic<uint64> _FirstIndex{ 0 };

	//The current write index.
	Atomic<uint64> _WriteIndex{ 0 };

	//The last index in the queue.
	Atomic<uint64> _LastIndex{ 0 };

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
	//The number of queued items.
	Atomic<uint64> _NumberOfQueuedItems{ 0 };
#endif

};
#endif