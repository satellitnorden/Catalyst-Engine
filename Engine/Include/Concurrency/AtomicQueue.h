#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

/*
*	Multiple producer - multiple consumers queue.
*	WILL overwrite values once the circular buffer has reached the beginning again.
*	This is to avoid reallocations when pushing/popping, so the atomic queue MUST be initialized with large enough storage to ensure that this never happens.
*/

template <typename TYPE, uint64 SIZE>
class AtomicQueue final
{

public:

	/*
	*	Pushes a new value into the queue.
	*/
	void Push(TYPE new_value) NOEXCEPT
	{
		uint64 old_write_index;
		uint64 new_write_index;

		do
		{
			old_write_index = _WriteIndex.load();

			new_write_index = old_write_index < (SIZE - 1) ? old_write_index + 1 : 0;
		} while (!_WriteIndex.compare_exchange_weak(old_write_index, new_write_index));

		_Queue[old_write_index] = new_value;

		uint64 expected_last_index{ new_write_index > 0 ? new_write_index - 1 : SIZE - 1 };
		uint64 new_last_index{ expected_last_index < (SIZE - 1) ? expected_last_index + 1 : 0 };

		while (!_LastIndex.compare_exchange_weak(expected_last_index, new_last_index));
	}

	/*
	*	Pops a value from the queue, if the queue is not empty. If the pop was succesful, a pointer to the popped value is returned, otherwise nullptr.
	*/
	RESTRICTED TYPE*const RESTRICT Pop() NOEXCEPT
	{
		uint64 old_first_index;
		uint64 old_last_index;
		uint64 new_first_index;

		do
		{
			old_first_index = _FirstIndex.load();
			old_last_index = _LastIndex.load();

			if (old_first_index == old_last_index)
			{
				return nullptr;
			}

			new_first_index = old_first_index < (SIZE - 1) ? old_first_index + 1 : 0;
		} while (!_FirstIndex.compare_exchange_weak(old_first_index, new_first_index));

		return &_Queue[old_first_index];
	}

private:

	//The underlying queue.
	StaticArray<TYPE, SIZE> _Queue;

	//The first index in the queue.
	std::atomic<uint64> _FirstIndex{ 0 };

	//The current write index.
	std::atomic<uint64> _WriteIndex{ 0 };

	//The last index in the queue.
	std::atomic<uint64> _LastIndex{ 0 };

};