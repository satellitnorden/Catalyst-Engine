#pragma once

//Engine core.
#include <EngineCore.h>

/*
*	Single producer - multiple consumers queue.
*	WILL overwrite values once the circular buffer has reached the beginning again.
*/

template <class ValueType>
class AtomicQueue final
{

public:

	/*
	*	Default constructor.
	*/
	AtomicQueue() CATALYST_NOEXCEPT
	{
		
	}

	/*
	*	Default destructor.
	*/
	~AtomicQueue() CATALYST_NOEXCEPT
	{
		//Deallocate the memory for the queue.
		MemoryUtilities::FreeMemory(queue);
	}

	/*
	*	Initializes this atomic queue.
	*/
	void Initialize(const size_t &newQueueSize) CATALYST_NOEXCEPT
	{
		//Allocate memory for the queue.
		queue = static_cast<ValueType*>(MemoryUtilities::AllocateMemory(sizeof(ValueType) * newQueueSize));

		//Set the queue size.
		queueSize = newQueueSize;
	}

	/*
	*	Pushes a new value into the queue.
	*/
	void Push(ValueType newValue) CATALYST_NOEXCEPT
	{
		queue[lastIndex] = newValue;

		lastIndex = lastIndex < (queueSize - 1) ? lastIndex + 1 : 0;
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. Returns if the pop was succesful.
	*/
	bool PopIfNotEmpty(ValueType &poppedValue) CATALYST_NOEXCEPT
	{
		ValueType temporaryValue;
		size_t oldFirstIndex;
		size_t oldLastIndex;
		size_t newFirstIndex;

		do
		{
			oldFirstIndex = firstIndex.load();
			oldLastIndex = lastIndex.load();

			if (oldFirstIndex == oldLastIndex)
				return false;

			temporaryValue = queue[oldFirstIndex];

			newFirstIndex = oldFirstIndex < (queueSize - 1) ? oldFirstIndex + 1 : 0;
		} while (!firstIndex.compare_exchange_weak(oldFirstIndex, newFirstIndex));

		poppedValue = temporaryValue;

		return true;
	}

private:

	//The underlying queue.
	ValueType *CATALYST_RESTRICT queue;

	//The size of the queue.
	size_t queueSize;

	//The first index in the queue.
	std::atomic<size_t> firstIndex{ 0 };

	//The last index in the queue.
	std::atomic<size_t> lastIndex{ 0 };

};