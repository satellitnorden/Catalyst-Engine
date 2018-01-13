#pragma once

template <class ObjectType>
class DynamicArray
{

public:

	/*
	*	Default constructor.
	*/
	CATALYST_NOALIAS DynamicArray() CATALYST_NOEXCEPT
		:
		array(nullptr),
		size(0),
		capacity(0)
	{

	}

	/*
	*	Constructor taking an initializer list.
	*/
	CATALYST_NOALIAS DynamicArray(std::initializer_list<ObjectType> &&initializerList) CATALYST_NOEXCEPT
	{
		//Reserve memory for all the elements in the initializer list.
		Reserve(initializerList.size());

		//Copy all elements of the initializer list to this dynamic array.
		memcpy(array, initializerList.begin(), sizeof(ObjectType) * capacity);

		//Set the size equal to the capacity.
		size = capacity;
	}

	/*
	*	Constructor taking the initial capacity as it's argument.
	*/
	CATALYST_NOALIAS DynamicArray(const size_t initialCapacity) CATALYST_NOEXCEPT
		:
		size(0)
	{
		//Reserve the initial capacity.
		Reserve(initialCapacity);
	}

	/*
	*	Copy constructor.
	*/
	CATALYST_NOALIAS DynamicArray(const DynamicArray &otherDynamicArray) CATALYST_NOEXCEPT
	{
		Reserve(otherDynamicArray.capacity);

		memcpy(array, otherDynamicArray.array, capacity);

		size = capacity;
	}

	/*
	*	Default destructor.
	*/
	CATALYST_NOALIAS ~DynamicArray() CATALYST_NOEXCEPT
	{
		//Call the destructor on all objects in the array.
		for (size_t i = 0; i < size; ++i)
		{
			array[i].~ObjectType();
		}

		//Free the memory used by the array.
		free(static_cast<void*>(array));
	}

	/*
	*	Copy assignment operator overload.
	*/
	CATALYST_NOALIAS void operator=(const DynamicArray &otherDynamicArray) CATALYST_NOEXCEPT
	{
		Reserve(otherDynamicArray.capacity);
		size = capacity;

		for (size_t i = 0; i < size; ++i)
		{
			array[i] = otherDynamicArray.array[i];
		}
	}

	/*
	*	Subscript operator overload, const.
	*/
	CATALYST_NOALIAS const ObjectType& operator[](const size_t index) const CATALYST_NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	CATALYST_NOALIAS ObjectType& operator[](const size_t index) CATALYST_NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED const ObjectType* begin() const  CATALYST_NOEXCEPT
	{
		return array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED ObjectType* begin()  CATALYST_NOEXCEPT
	{
		return array;
	}

	/*
	*	End iterator, const.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED const ObjectType* end() const CATALYST_NOEXCEPT
	{
		return array + size;
	}

	/*
	*	End iterator, non-const.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED ObjectType* end() CATALYST_NOEXCEPT
	{
		return array + size;
	}

	/*
	*	Returns the capacity of this dynamic array.
	*/
	CATALYST_NOALIAS size_t Capacity() const CATALYST_NOEXCEPT { return capacity; }

	/*
	*	Returns the size of this dynamic array.
	*/
	CATALYST_NOALIAS size_t Size() const CATALYST_NOEXCEPT { return size; }

	/*
	*	Returns whether or not this dynamic array is empty.
	*/
	CATALYST_NOALIAS bool Empty() const CATALYST_NOEXCEPT { return size == 0; }

	/*
	*	Returns a pointer to the data of this dynamic array, const.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED const ObjectType* Data() const CATALYST_NOEXCEPT
	{
		return CATALYST_LIKELY(array) ? array : nullptr;
	}

	/*
	*	Returns a pointer to the data of this dynamic array, non-const.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED ObjectType* Data() CATALYST_NOEXCEPT
	{
		return CATALYST_LIKELY(array) ? array : nullptr;
	}

	/*
	*	Returns the back of this dynamic array, const.
	*/
	CATALYST_NOALIAS const ObjectType& Back() const CATALYST_NOEXCEPT
	{
		return array[size - 1];
	}

	/*
	*	Returns the back of this dynamic array, non-const.
	*/
	CATALYST_NOALIAS ObjectType& Back() CATALYST_NOEXCEPT
	{
		return array[size - 1];
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array.
	*/
	template <class... Arguments>
	CATALYST_NOALIAS void Emplace(Arguments&&... arguments) CATALYST_NOEXCEPT
	{
		if (size >= capacity)
		{
			Reserve(size > 0 ? size * 3 : 16);
		}

		new (&array[size++]) ObjectType(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array without first checking if the array has the required capacity.
	*/
	template <class... Arguments>
	CATALYST_NOALIAS void EmplaceUnsafe(Arguments&&... arguments) CATALYST_NOEXCEPT
	{
		new (&array[size++]) ObjectType(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Finds and erases an element in the array. Does not respect order of elements.
	*/
	void Erase(const ObjectType &objectToErase) CATALYST_NOEXCEPT
	{
		for (auto object : *this)
		{
			if (object == objectToErase)
			{
				object = std::move(Back());
				Pop();

				return;
			}
		}
	}

	/*
	*	Finds an element and returns it, const.
	*/
	const Optional<ObjectType> Find(const ObjectType &objectToFind) const CATALYST_NOEXCEPT
	{
		for (auto object : *this)
		{
			if (object == objectToFind)
			{
				return Optional<ObjectType>(object);
			}
		}

		return Optional<ObjectType>();
	}

	/*
	*	Finds an element and returns it, non-const.
	*/
	Optional<ObjectType> Find(const ObjectType &objectToFind) CATALYST_NOEXCEPT
	{
		for (auto object : *this)
		{
			if (object == objectToFind)
			{
				return Optional<ObjectType>(object);
			}
		}

		return Optional<ObjectType>();
	}

	/*
	*	Pops an element from the back of this dynamic array.
	*/
	CATALYST_NOALIAS void Pop() CATALYST_NOEXCEPT
	{
		--size;
	}

	/*
	*	Reserves a new chunk of memory, changing the array's capacity.
	*/
	CATALYST_NOALIAS void Reserve(const size_t newCapacity) CATALYST_NOEXCEPT
	{
		//Allocate the new array.
		ObjectType *CATALYST_RESTRICT newArray{ static_cast<ObjectType*>(malloc(sizeof(ObjectType) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		memcpy(newArray, array, sizeof(ObjectType) * size);

		//Free the old array.
		free(static_cast<void*>(array));

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
	}

	/*
	*	Resizes this dynamic array, filling it with default constructed objects.
	*/
	CATALYST_NOALIAS void Resize(const size_t newCapacity) CATALYST_NOEXCEPT
	{
		//Allocate the new array.
		ObjectType *CATALYST_RESTRICT newArray{ static_cast<ObjectType*>(malloc(sizeof(ObjectType) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		memcpy(newArray, array, sizeof(ObjectType) * (size < newCapacity ? size : newCapacity));

		//Default construct the remaining objects.
		for (size_t i = size; i < newCapacity; ++i)
		{
			new (&newArray[i]) ObjectType;
		}

		//Free the old array.
		free(static_cast<void*>(array));

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
		size = newCapacity;
	}

private:

	//Pointer to the current array.
	ObjectType *CATALYST_RESTRICT array;

	//The current size of this dynamic array.
	size_t size;

	//The current capacity of this dynamic array.
	size_t capacity;

};