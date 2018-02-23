#pragma once

template <class ObjectType, uint64 InitialCapacity = 0>
class DynamicArray final
{

public:

	/*
	*	Default constructor.
	*/
	DynamicArray() NOEXCEPT
		:
		size(0)
	{
		if (InitialCapacity > 0)
		{
			ReserveConstruct(InitialCapacity);
		}

		else
		{
			array = nullptr;
			capacity = 0;
		}
	}

	/*
	*	Constructor taking an initializer list.
	*/
	DynamicArray(std::initializer_list<ObjectType> &&initializerList) NOEXCEPT
	{
		//Reserve memory for all the elements in the initializer list.
		ReserveConstruct(initializerList.size());

		//Copy all elements of the initializer list to this dynamic array.
		MemoryUtilities::CopyMemory(array, initializerList.begin(), sizeof(ObjectType) * capacity);

		//Set the size equal to the capacity.
		size = capacity;
	}

	/*
	*	Copy constructor.
	*/
	DynamicArray(const DynamicArray &otherDynamicArray) NOEXCEPT
	{
		ReserveConstruct(otherDynamicArray.capacity);

		MemoryUtilities::CopyMemory(array, otherDynamicArray.array, sizeof(ObjectType) * capacity);

		size = capacity;
	}

	/*
	*	Move constructor.
	*/
	DynamicArray(DynamicArray &&otherDynamicArray) NOEXCEPT
	{
		array = otherDynamicArray.array;
		size = otherDynamicArray.size;
		capacity = otherDynamicArray.capacity;

		otherDynamicArray.array = nullptr;
	}

	/*
	*	Default destructor.
	*/
	~DynamicArray() NOEXCEPT
	{
		//This dynamic array might have been moved from, thus we need to test the array pointer.
		if (array)
		{
			//Call the destructor on all objects in the array.
			for (uint64 i = 0; i < size; ++i)
			{
				array[i].~ObjectType();
			}

			//Free the memory used by the array.
			MemoryUtilities::FreeMemory(static_cast<void *RESTRICT>(array));
		}
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const DynamicArray &otherDynamicArray) NOEXCEPT
	{
		Reserve(otherDynamicArray.capacity);
		size = capacity;

		for (uint64 i = 0; i < size; ++i)
		{
			array[i] = otherDynamicArray.array[i];
		}
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(DynamicArray &&otherDynamicArray) NOEXCEPT
	{
		array = otherDynamicArray.array;
		size = otherDynamicArray.size;
		capacity = otherDynamicArray.capacity;

		otherDynamicArray.array = nullptr;
	}

	/*
	*	Subscript operator overload, const.
	*/
	const ObjectType& operator[](const uint64 index) const NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	ObjectType& operator[](const uint64 index) NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const ObjectType* begin() const  NOEXCEPT
	{
		return array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED ObjectType* begin()  NOEXCEPT
	{
		return array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const ObjectType* end() const NOEXCEPT
	{
		return array + size;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED ObjectType* end() NOEXCEPT
	{
		return array + size;
	}

	/*
	*	Returns the capacity of this dynamic array.
	*/
	uint64 Capacity() const NOEXCEPT { return capacity; }

	/*
	*	Returns the size of this dynamic array.
	*/
	uint64 Size() const NOEXCEPT { return size; }

	/*
	*	Returns whether or not this dynamic array is empty.
	*/
	bool Empty() const NOEXCEPT { return size == 0; }

	/*
	*	Returns a pointer to the data of this dynamic array, const.
	*/
	RESTRICTED const ObjectType* Data() const NOEXCEPT
	{
		return LIKELY(array) ? array : nullptr;
	}

	/*
	*	Returns a pointer to the data of this dynamic array, non-const.
	*/
	RESTRICTED ObjectType* Data() NOEXCEPT
	{
		return LIKELY(array) ? array : nullptr;
	}

	/*
	*	Returns the back of this dynamic array, const.
	*/
	const ObjectType& Back() const NOEXCEPT
	{
		return array[size - 1];
	}

	/*
	*	Returns the back of this dynamic array, non-const.
	*/
	ObjectType& Back() NOEXCEPT
	{
		return array[size - 1];
	}

	/*
	*	Clears this dynamic array of elements without calling the destructor on the underlying elements.
	*/
	void ClearFast() NOEXCEPT
	{
		size = 0;
	}

	/*
	*	Clears this dynamic array of elements, calling the destructor on the underlying elements.
	*/
	void ClearSlow() NOEXCEPT
	{
		for (uint64 i = 0; i < size; ++i)
		{
			array[i].~ÕbjectType();
		}

		size = 0;
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array.
	*/
	template <class... Arguments>
	void EmplaceSlow(Arguments&&... arguments) NOEXCEPT
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
	void EmplaceFast(Arguments&&... arguments) NOEXCEPT
	{
		new (&array[size++]) ObjectType(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Finds and erases an element in the array. Does not respect order of elements.
	*/
	void Erase(const ObjectType &objectToErase) NOEXCEPT
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
	const Optional<ObjectType> Find(const ObjectType &objectToFind) const NOEXCEPT
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
	Optional<ObjectType> Find(const ObjectType &objectToFind) NOEXCEPT
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
	void Pop() NOEXCEPT
	{
		--size;
	}

	/*
	*	Reserves a new chunk of memory, changing the array's capacity.
	*/
	void Reserve(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		ObjectType *RESTRICT newArray{ static_cast<ObjectType*>(MemoryUtilities::AllocateMemory(sizeof(ObjectType) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, array, sizeof(ObjectType) * size);

		//Free the old array.
		MemoryUtilities::FreeMemory(static_cast<void*>(array));

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
	}

	/*
	*	Resizes this dynamic array, filling it with default constructed objects.
	*/
	void Resize(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		ObjectType *RESTRICT newArray{ static_cast<ObjectType*>(MemoryUtilities::AllocateMemory(sizeof(ObjectType) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, array, sizeof(ObjectType) * (size < newCapacity ? size : newCapacity));

		//Default construct the remaining objects.
		for (uint64 i = size; i < newCapacity; ++i)
		{
			new (&newArray[i]) ObjectType;
		}

		//Free the old array.
		MemoryUtilities::FreeMemory(static_cast<void*>(array));

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
		size = newCapacity;
	}

private:

	//Pointer to the current array.
	ObjectType *RESTRICT array;

	//The current size of this dynamic array.
	uint64 size;

	//The current capacity of this dynamic array.
	uint64 capacity;

	/*
	*	Reserves a new chunk of memory, changing the array's capacity, without copying over the old array.
	*/
	void ReserveConstruct(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		array = static_cast<ObjectType*>(MemoryUtilities::AllocateMemory(sizeof(ObjectType) * newCapacity));

		//Update the capacity.
		capacity = newCapacity;
	}

};