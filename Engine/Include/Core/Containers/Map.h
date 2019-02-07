#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Pair.h>

template <typename KeyType, typename ObjectType>
class Map final
{

public:

	/*
	*	Subsript operator overload, non-const.
	*/
	ObjectType& operator[](const KeyType key)
	{
		//First, see if the object exists in the map.
		if (ObjectType *RESTRICT object{ Find(key) })
		{
			//If the object existed, just return it.
			return *object;
		}

		//Else, create a new object and return it.
		else
		{
			EmplaceSlow(key, ObjectType());

			return *Find(key);
		}
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const Pair<KeyType, ObjectType> *const RESTRICT begin() const  NOEXCEPT
	{
		return _Map.begin();
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED Pair<KeyType, ObjectType> *const RESTRICT begin()  NOEXCEPT
	{
		return _Map.begin();
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const Pair<KeyType, ObjectType> *const RESTRICT end() const NOEXCEPT
	{
		return _Map.end();
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED Pair<KeyType, ObjectType> *const RESTRICT end() NOEXCEPT
	{
		return _Map.end();
	}

	/*
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, const.
	*/
	const ObjectType *const RESTRICT Find(const KeyType key) const NOEXCEPT
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
	ObjectType *const RESTRICT Find(const KeyType key) NOEXCEPT
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
	void EmplaceSlow(const KeyType newKey, const ObjectType &newObject)
	{
		_Map.EmplaceSlow(newKey, newObject);
	}

private:

	//The underlying dynamic array.
	DynamicArray<Pair<KeyType, ObjectType>> _Map;

};