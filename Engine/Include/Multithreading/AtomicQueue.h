#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

/*
*	Multiple producer - multiple consumers queue.
*	WILL overwrite values once the circular buffer has reached the beginning again.
*	This is to avoid reallocations when pushing/popping, so the atomic queue MUST be initialized with large enough storage to ensure that this never happens.
*/

template <typename Type, uint64 SIZE>
class AtomicQueue final
{

public:

	/*
	*	Pushes a new value into the queue.
	*/
	void Push(Type newValue) NOEXCEPT
	{
		uint64 oldWriteIndex;
		uint64 newWriteIndex;

		do
		{
			oldWriteIndex = _WriteIndex.load();

			newWriteIndex = oldWriteIndex < (SIZE - 1) ? oldWriteIndex + 1 : 0;
		} while (!_WriteIndex.compare_exchange_weak(oldWriteIndex, newWriteIndex));

		_Queue[oldWriteIndex] = newValue;

		uint64 expectedLastIndex{ newWriteIndex > 0 ? newWriteIndex - 1 : SIZE - 1 };
		uint64 newLastIndex{ expectedLastIndex < (SIZE - 1) ? expectedLastIndex + 1 : 0 };

		while (!_LastIndex.compare_exchange_weak(expectedLastIndex, newLastIndex));
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	RESTRICTED Type *const RESTRICT Pop() NOEXCEPT
	{
		uint64 oldFirstIndex;
		uint64 oldLastIndex;
		uint64 newFirstIndex;

		do
		{
			oldFirstIndex = _FirstIndex.load();
			oldLastIndex = _LastIndex.load();

			if (oldFirstIndex == oldLastIndex)
				return nullptr;

			newFirstIndex = oldFirstIndex < (SIZE - 1) ? oldFirstIndex + 1 : 0;
		} while (!_FirstIndex.compare_exchange_weak(oldFirstIndex, newFirstIndex));

		return &_Queue[oldFirstIndex];
	}

private:

	//The underlying queue.
	StaticArray<Type, SIZE> _Queue;

	//The first index in the queue.
	std::atomic<uint64> _FirstIndex{ 0 };

	//The current write index.
	std::atomic<uint64> _WriteIndex{ 0 };

	//The last index in the queue.
	std::atomic<uint64> _LastIndex{ 0 };

};