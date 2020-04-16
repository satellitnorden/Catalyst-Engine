#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Pair.h>

template <typename KEY_TYPE, typename VALUE_TYPE>
class HashTable final
{

public:

	/*
	*	Adds the given key/value pair.
	*/
	FORCE_INLINE void Add(const KEY_TYPE &key, const VALUE_TYPE &value) NOEXCEPT
	{
		_Elements.Emplace(key, value);
	}

	/*
	*	Finds the value with the given key and returns a pointer to it, or nullptr if none was found.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD VALUE_TYPE *const RESTRICT Find(const KEY_TYPE &key) NOEXCEPT
	{
		for (Pair<KEY_TYPE, VALUE_TYPE> &element : _Elements)
		{
			if (element._First == key)
			{
				return &element._Second;
			}
		}

		return nullptr;
	}

private:

	//The underlying elements. TODO: Change this.
	DynamicArray<Pair<KEY_TYPE, VALUE_TYPE>> _Elements;

};