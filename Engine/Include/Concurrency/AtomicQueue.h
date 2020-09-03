#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Concurrency.
#include <Concurrency/Atomic.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Constants.
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

	static_assert(CatalystBaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		//Cache the current last index.
		const uint64 current_last_index{ _LastIndex.load(std::memory_order_acquire) };

		//Write the new value into the last index.
		_Queue[current_last_index] = new_value;

		//Increment the last index.
		_LastIndex.store((current_last_index + 1) & (SIZE - 1), std::memory_order_release);

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		++_NumberOfQueuedItems;

		ASSERT(_NumberOfQueuedItems < SIZE, "Number of queued items is too high! Number of queued items: " << _NumberOfQueuedItems << ", SIZE: " << SIZE << ".");
#endif
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Pop() NOEXCEPT
	{
		//Cache the current first and last index.
		const uint64 current_first_index{ _FirstIndex.load(std::memory_order_acquire) };
		const uint64 current_last_index{ _LastIndex.load(std::memory_order_acquire) };

		//If the first and last index are the same, the queue is empty.
		if (current_first_index == current_last_index)
		{
			return nullptr;
		}

		//Cache the value at the current first index.
		TYPE* const RESTRICT current_first_value{ &_Queue[current_first_index] };

		//Increment the first index.
		_FirstIndex.store((current_first_index + 1) & (SIZE - 1), std::memory_order_release);

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		--_NumberOfQueuedItems;
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

	static_assert(CatalystBaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE &new_value) NOEXCEPT
	{
		//Cache the current last index.
		const uint64 current_last_index{ _LastIndex.load(std::memory_order_acquire) };

		//Write the new value into the last index.
		_Queue[current_last_index] = new_value;

		//Increment the last index.
		_LastIndex.store((current_last_index + 1) & (SIZE - 1), std::memory_order_release);

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		++_NumberOfQueuedItems;

		ASSERT(_NumberOfQueuedItems < SIZE, "Number of queued items is too high! Number of queued items: " << _NumberOfQueuedItems << ", SIZE: " << SIZE << ".");
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
			old_first_index = _FirstIndex.load(std::memory_order_acquire);
			old_last_index = _LastIndex.load(std::memory_order_acquire);

			if (old_first_index == old_last_index)
			{
				return nullptr;
			}

			new_first_index = old_first_index < (SIZE - 1) ? old_first_index + 1 : 0;
		} while (!_FirstIndex.compare_exchange_weak(old_first_index, new_first_index, std::memory_order_release));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		--_NumberOfQueuedItems;
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

	static_assert(CatalystBaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

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
			old_write_index = _WriteIndex.load(std::memory_order_acquire);

			new_write_index = (old_write_index + 1) & (SIZE - 1);
		} while (!_WriteIndex.compare_exchange_weak(old_write_index, new_write_index, std::memory_order_release));

		_Queue[old_write_index] = new_value;

		uint64 expected_last_index{ new_write_index > 0 ? new_write_index - 1 : SIZE - 1 };
		uint64 new_last_index{ (expected_last_index + 1) & (SIZE - 1) };

		while (!_LastIndex.compare_exchange_weak(expected_last_index, new_last_index, std::memory_order_release));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		++_NumberOfQueuedItems;

		ASSERT(_NumberOfQueuedItems < SIZE, "Number of queued items is too high! Number of queued items: " << _NumberOfQueuedItems << ", SIZE: " << SIZE << ".");
#endif
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Pop() NOEXCEPT
	{
		//Cache the current first and last index.
		const uint64 current_first_index{ _FirstIndex.load(std::memory_order_acquire) };
		const uint64 current_last_index{ _LastIndex.load(std::memory_order_acquire) };

		//If the first and last index are the same, the queue is empty.
		if (current_first_index == current_last_index)
		{
			return nullptr;
		}

		//Cache the value at the current first index.
		TYPE* const RESTRICT current_first_value{ &_Queue[current_first_index] };

		//Increment the first index.
		_FirstIndex.store((current_first_index + 1) & (SIZE - 1), std::memory_order_release);

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		--_NumberOfQueuedItems;
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

	static_assert(CatalystBaseMath::IsPowerOfTwo(SIZE), "SIZE is not a power of two!");

public:

	/*
	*	Pushes a new value into the queue.
	*/
	FORCE_INLINE void Push(const TYPE& new_value) NOEXCEPT
	{
		uint64 old_write_index;
		uint64 new_write_index;

		do
		{
			old_write_index = _WriteIndex.load(std::memory_order_acquire);

			new_write_index = old_write_index < (SIZE - 1) ? old_write_index + 1 : 0;
		} while (!_WriteIndex.compare_exchange_weak(old_write_index, new_write_index, std::memory_order_release));

		_Queue[old_write_index] = new_value;

		uint64 expected_last_index{ new_write_index > 0 ? new_write_index - 1 : SIZE - 1 };
		uint64 new_last_index{ expected_last_index < (SIZE - 1) ? expected_last_index + 1 : 0 };

		while (!_LastIndex.compare_exchange_weak(expected_last_index, new_last_index, std::memory_order_release));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		++_NumberOfQueuedItems;

		ASSERT(_NumberOfQueuedItems < SIZE, "Number of queued items is too high! Number of queued items: " << _NumberOfQueuedItems << ", SIZE: " << SIZE << ".");
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
			old_first_index = _FirstIndex.load(std::memory_order_acquire);
			old_last_index = _LastIndex.load(std::memory_order_acquire);

			if (old_first_index == old_last_index)
			{
				return nullptr;
			}

			new_first_index = old_first_index < (SIZE - 1) ? old_first_index + 1 : 0;
		} while (!_FirstIndex.compare_exchange_weak(old_first_index, new_first_index, std::memory_order_release));

#if VALIDATE_NUMBER_OF_QUEUED_ITEMS
		--_NumberOfQueuedItems;
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