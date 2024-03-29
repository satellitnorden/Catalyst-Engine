#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

template <uint64 SIZE>
class Any final
{

public:

	/*
	*	Returns the data as the given type, const
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD const TYPE *const RESTRICT Get() const NOEXCEPT
	{
		static_assert(sizeof(TYPE) <= SIZE, "Size of TYPE is greater than the allocated size, this is not allowed!");

		return static_cast<const TYPE *const RESTRICT>(static_cast<const void *const RESTRICT>(_Data.Data()));
	}

	/*
	*	Returns the data as the given type, non-const
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Get() NOEXCEPT
	{
		static_assert(sizeof(TYPE) <= SIZE, "Size of TYPE is greater than the allocated size, this is not allowed!");

		return static_cast<TYPE *const RESTRICT>(static_cast<void *const RESTRICT>(_Data.Data()));
	}

	/*
	*	Sets the data as the given type, non-const
	*/
	template <typename TYPE>
	FORCE_INLINE void Set(const TYPE &value) NOEXCEPT
	{
		static_assert(sizeof(TYPE) <= SIZE, "Size of TYPE is greater than the allocated size, this is not allowed!");

		*static_cast<TYPE *const RESTRICT>(static_cast<void *const RESTRICT>(_Data.Data())) = value;
	}

private:
	
	//The data.
	StaticArray<byte, SIZE> _Data;

};