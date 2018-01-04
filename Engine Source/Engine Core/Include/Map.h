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
		//Mark this map as unsorted.
		isSorted = false;

		//Emplace the new pair.
		map.Emplace(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Finds the pair with the key, returns a pointer to the value.
	*/
	CATALYST_RESTRICTED ValueType* Find(const KeyType key) CATALYST_NOEXCEPT
	{
		//If the map is not sorted already, sort it.
		if (!isSorted)
		{
			Sort();
		}
		
		//Find the right pair with a binary search.
		size_t currentIndex = map.Size() / 2;
		size_t step = currentIndex / 2;

		while (step > 0)
		{
			//Search in the upper half.
			if (map[currentIndex].key < key)
			{
				currentIndex += step;
				step /= 2;
			}

			//Search in the lower half.
			else if (map[currentIndex].key < key)
			{
				currentIndex -= step;
				step /= 2;
			}

			//Found the right pair!
			else
			{
				return &map[currentIndex].value;
			}
		}

		//A pair with this key does not exist.
		return nullptr;
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

	//Keeps track of whether or not the map is sorted.
	bool isSorted{ true };

	/*
	*	Sorts the underlying map.
	*/
	void Sort() CATALYST_NOEXCEPT
	{
		std::sort(map.begin(), map.end());

		isSorted = true;
	}

};