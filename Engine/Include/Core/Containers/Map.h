#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Pair.h>

template <typename KEY_TYPE, typename OBJECT_TYPE>
class Map final
{

public:

	/*
	*	Subsript operator overload, non-const.
	*/
	FORCE_INLINE NO_DISCARD OBJECT_TYPE& operator[](const KEY_TYPE key)
	{
		//First, see if the object exists in the map.
		if (OBJECT_TYPE *RESTRICT object{ Find(key) })
		{
			//If the object existed, just return it.
			return *object;
		}

		//Else, create a new object and return it.
		else
		{
			EmplaceSlow(key, OBJECT_TYPE());

			return *Find(key);
		}
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Pair<KEY_TYPE, OBJECT_TYPE> *const RESTRICT begin() const  NOEXCEPT
	{
		return _Map.begin();
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Pair<KEY_TYPE, OBJECT_TYPE> *const RESTRICT begin()  NOEXCEPT
	{
		return _Map.begin();
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const Pair<KEY_TYPE, OBJECT_TYPE> *const RESTRICT end() const NOEXCEPT
	{
		return _Map.end();
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Pair<KEY_TYPE, OBJECT_TYPE> *const RESTRICT end() NOEXCEPT
	{
		return _Map.end();
	}

	/*
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const OBJECT_TYPE *const RESTRICT Find(const KEY_TYPE key) const NOEXCEPT
	{
		for (auto &mapEntry : _Map)
		{
			if (mapEntry._First == key)
			{
				return &mapEntry._Second;
			}
		}

		return nullptr;
	}

	/*
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD OBJECT_TYPE *const RESTRICT Find(const KEY_TYPE key) NOEXCEPT
	{
		for (auto &mapEntry : _Map)
		{
			if (mapEntry._First == key)
			{
				return &mapEntry._Second;
			}
		}

		return nullptr;
	}

	/*
	*	Emplaces a new pair into the map.
	*/
	FORCE_INLINE void EmplaceSlow(const KEY_TYPE newKey, const OBJECT_TYPE &newObject)
	{
		_Map.EmplaceSlow(newKey, newObject);
	}

	/*
	*	Returns the size of this map.
	*/
	FORCE_INLINE uint64 Size() const NOEXCEPT
	{
		return _Map.Size();
	}

private:

	//The underlying dynamic array.
	DynamicArray<Pair<KEY_TYPE, OBJECT_TYPE>> _Map;

};