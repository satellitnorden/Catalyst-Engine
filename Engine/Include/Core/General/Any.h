#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

template <uint64 SIZE>
class Any final
{

public:

	/*
	*	Returns the data as the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Get() NOEXCEPT
	{
		static_assert(sizeof(TYPE) <= SIZE, "Size of TYPE is greater than the allocated size, this is not allowed!");

		return static_cast<TYPE *const RESTRICT>(static_cast<void *const RESTRICT>(_Data.Data()));
	}

private:
	
	//The data.
	StaticArray<byte, SIZE> _Data;

};