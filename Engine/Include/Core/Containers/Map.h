#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Pair.h>

template <class KeyType, class ObjectType>
class Map final
{

public:

	/*
	*	Default constructor.
	*/
	Map() NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~Map() NOEXCEPT
	{

	}

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
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, const.
	*/
	const ObjectType* RESTRICT Find(const KeyType key) const NOEXCEPT
	{
		for (auto &mapEntry : map)
		{
			if (mapEntry.GetFirstValue() == key)
			{
				return &mapEntry.GetSecondValue();
			}
		}

		return nullptr;
	}

	/*
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, non-const.
	*/
	ObjectType* RESTRICT Find(const KeyType key) NOEXCEPT
	{
		for (auto &mapEntry : map)
		{
			if (mapEntry.GetFirstValue() == key)
			{
				return &mapEntry.GetSecondValue();
			}
		}

		return nullptr;
	}

	/*
	*	Emplaces a new pair into the map.
	*/
	void EmplaceSlow(const KeyType newKey, const ObjectType &newObject)
	{
		map.EmplaceSlow(newKey, newObject);
	}

private:

	//The underlying dynamic array.
	DynamicArray<Pair<KeyType, ObjectType>> map;

};