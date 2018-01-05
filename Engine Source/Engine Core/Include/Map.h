#pragma once

template <class KeyType, class ValueType>
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
	*	Constructs a new pair directly in memory.
	*/
	template <class... Arguments>
	void Emplace(Arguments&&... arguments) CATALYST_NOEXCEPT
	{
		//Emplace the new pair.
		map.Emplace(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Finds the pair with the key, returns a pointer to the value.
	*/
	CATALYST_RESTRICTED ValueType* Find(const KeyType key) CATALYST_NOEXCEPT
	{
		//For now, only do a regular linear search.
		return LinearSearch(key);
	}

private:

	/*
	*	Pair class implementation.
	*/
	template <class KeyType, class ValueType>
	class Pair
	{

	public:

		//The underlying key.
		KeyType key;

		//The underlying value.
		ValueType value;

		/*
		*	Default constructor - prohibited, must be constructed with the proper arguments.
		*/
		Pair() CATALYST_NOEXCEPT = delete;

		/*
		*	Constructor taking the key and the value as arguments.
		*/
		Pair(const KeyType &newKey, const ValueType &newValue) CATALYST_NOEXCEPT
			:
			key(newKey),
			value(newValue)
		{

		}

		/*
		*	Default destructor.
		*/
		~Pair() CATALYST_NOEXCEPT
		{

		}

		/*
		*	Greater than operator overload.
		*/
		bool operator>(const Pair<KeyType, ValueType> &otherPair) CATALYST_NOEXCEPT
		{
			return key > otherPair.key;
		}

		/*
		*	Less than operator overload.
		*/
		bool operator<(const Pair<KeyType, ValueType> &otherPair) CATALYST_NOEXCEPT
		{
			return key < otherPair.key;
		}

	};

	//The underlying array.
	DynamicArray< Pair<KeyType, ValueType> > map;

	/*
	*	Sorts the underlying map.
	*/
	void Sort() CATALYST_NOEXCEPT
	{
		std::sort(map.begin(), map.end());

		isSorted = true;
	}

	/*
	*	Performs a binary search for a given key and returns a pointer to the value associated with the key.
	*/
	CATALYST_RESTRICTED ValueType* BinarySearch(const KeyType key) CATALYST_NOEXCEPT
	{
		//If the map is not sorted already, sort it.
		if (!isSorted)
		{
			Sort();
		}

		size_t minimum = 0;
		size_t maximum = map.Size();

		while (maximum >= minimum)
		{
			size_t middle = (maximum + minimum) / 2;

			if (map[middle].key < key)
			{
				minimum = middle + 1;
			}

			else if (map[middle].key > key)
			{
				maximum = middle - 1;
			}

			else
			{
				return &map[middle].value;
			}
		}
		
		//A pair with this key does not exist.
		return nullptr;
	}

	/*
	*	Performs a linear search for a given key and returns a pointer to the value associated with the key.
	*/
	CATALYST_RESTRICTED ValueType* LinearSearch(const KeyType key) CATALYST_NOEXCEPT
	{
		for (auto &pair : map)
		{
			if (pair.key == key)
			{
				return &pair.value;
			}
		}

		return nullptr;
	}

};