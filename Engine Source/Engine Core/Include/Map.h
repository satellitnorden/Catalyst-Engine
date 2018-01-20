#pragma once

template <class KeyType, class ObjectType>
class Map
{

public:

	/*
	*	Default constructor.
	*/
	Map() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~Map() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, const.
	*/
	CATALYST_RESTRICTED const ObjectType* Find(const KeyType &keyToFind) const CATALYST_NOEXCEPT
	{
		for (auto &mapEntry : map)
		{
			if (mapEntry.GetFirstValue() == keyToFind)
			{
				return &mapEntry.GetSecondValue();
			}
		}

		return nullptr;
	}

	/*
	*	Finds the object associated with specific key and returns a pointer to that object. Returns nullptr if it can't find the object, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* Find(const KeyType &keyToFind) CATALYST_NOEXCEPT
	{
		for (auto &mapEntry : map)
		{
			if (mapEntry.GetFirstValue() == keyToFind)
			{
				return &mapEntry.GetSecondValue();
			}
		}

		return nullptr;
	}

	/*
	*	Emplaces a new pair into the map.
	*/
	void Emplace(const KeyType &newKey, const ObjectType &newObject)
	{
		map.Emplace(newKey, newObject);
	}

private:

	//The underlying dynamic array.
	DynamicArray<Pair<KeyType, ObjectType>> map;

};