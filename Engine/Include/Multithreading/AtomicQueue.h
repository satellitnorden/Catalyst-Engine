#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

/*
*	Multiple producer - multiple consumers queue.
*	WILL overwrite values once the circular buffer has reached the beginning again.
*	This is to avoid reallocations when pushing/popping, so the atomic queue MUST be initialized with large enough storage to ensure that this never happens.
*/

template <class ValueType>
class AtomicQueue final
{

public:

	/*
	*	Default constructor.
	*/
	AtomicQueue() NOEXCEPT
	{
		
	}

	/*
	*	Default destructor.
	*/
	~AtomicQueue() NOEXCEPT
	{
		//Deallocate the memory for the queue.
		MemoryUtilities::FreeMemory(queue);
	}

	/*
	*	Initializes this atomic queue.
	*/
	void Initialize(const uint64 &newQueueSize) NOEXCEPT
	{
		//Allocate memory for the queue.
		queue = static_cast<ValueType*>(MemoryUtilities::AllocateMemory(sizeof(ValueType) * newQueueSize));

		//Set the queue size.
		queueSize = newQueueSize;
	}

	/*
	*	Pushes a new value into the queue.
	*/
	void Push(ValueType newValue) NOEXCEPT
	{
		uint64 oldWriteIndex;
		uint64 newWriteIndex;

		do
		{
			oldWriteIndex = writeIndex.load();

			newWriteIndex = oldWriteIndex < (queueSize - 1) ? oldWriteIndex + 1 : 0;
		} while (!writeIndex.compare_exchange_weak(oldWriteIndex, newWriteIndex));

		queue[oldWriteIndex] = newValue;

		uint64 expectedLastIndex{ newWriteIndex > 0 ? newWriteIndex - 1 : queueSize - 1 };
		uint64 newLastIndex{ expectedLastIndex < (queueSize - 1) ? expectedLastIndex + 1 : 0 };

		while (!lastIndex.compare_exchange_weak(expectedLastIndex, newLastIndex));
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	RESTRICTED ValueType *const RESTRICT Pop() NOEXCEPT
	{
		uint64 oldFirstIndex;
		uint64 oldLastIndex;
		uint64 newFirstIndex;

		do
		{
			oldFirstIndex = firstIndex.load();
			oldLastIndex = lastIndex.load();

			if (oldFirstIndex == oldLastIndex)
				return nullptr;

			newFirstIndex = oldFirstIndex < (queueSize - 1) ? oldFirstIndex + 1 : 0;
		} while (!firstIndex.compare_exchange_weak(oldFirstIndex, newFirstIndex));

		return &queue[oldFirstIndex];
	}

private:

	//The underlying queue.
	ValueType *RESTRICT queue;

	//The size of the queue.
	uint64 queueSize;

	//The first index in the queue.
	std::atomic<uint64> firstIndex{ 0 };

	//The current write index.
	std::atomic<uint64> writeIndex{ 0 };

	//The last index in the queue.
	std::atomic<uint64> lastIndex{ 0 };

};