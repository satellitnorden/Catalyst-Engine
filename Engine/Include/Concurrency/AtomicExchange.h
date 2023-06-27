#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>

/*
*	This is a class built for scenarios where one thread, needs to pass a value to another thread, both operating at a different update interval.
*	The value should only be written from one thread and read from one other thread.
*	The value cannot be written to from thread A unless the previous value has been read from thread B.
*	Likewise, the value can only be read from thread B if it has been written by thread A.
*/
template <typename TYPE>
class AtomicExchange final
{

public:

	/*
	*	Returns if this exchange has a value.
	*/
	FORCE_INLINE NO_DISCARD bool HasValue() const NOEXCEPT
	{
		return _HasValue.IsSet();
	}

	/*
	*	Writes the value.
	*/
	FORCE_INLINE void Write(const TYPE &value) NOEXCEPT
	{
		//Wait for the value to be read.
		while (HasValue())
		{
			Concurrency::CurrentThread::Yield();
		}

		//Write the value.
		_Value = value;

		//Signal that the value has been written.
		_HasValue.Set();
	}

	/*
	*	Reads the value.
	*/
	FORCE_INLINE void Read(TYPE *const RESTRICT value) NOEXCEPT
	{
		ASSERT(HasValue(), "Trying to read a value that hasn't been written to!");

		//Set the value.
		*value = _Value;

		//Signal that the value can be written to again.
		_HasValue.Clear();
	}

private:

	//The value.
	TYPE _Value;

	//Denotes if this exchange has a value.
	AtomicFlag _HasValue;

};