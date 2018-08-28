#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/Utilities/MemoryUtilities.h>

template <typename Type>
class DynamicArray final
{

public:

	/*
	*	Default constructor.
	*/
	DynamicArray() NOEXCEPT
		:
		array(nullptr),
		size(0),
		capacity(0)
	{

	}

	/*
	*	Constructor taking an initializer list.
	*/
	DynamicArray(std::initializer_list<Type> &&initializerList) NOEXCEPT
	{
		//Reserve memory for all the elements in the initializer list.
		ReserveConstruct(initializerList.size());

		//Copy all elements of the initializer list to this dynamic array.
		MemoryUtilities::CopyMemory(array, initializerList.begin(), sizeof(Type) * capacity);

		//Set the size equal to the capacity.
		size = capacity;
	}

	/*
	*	Copy constructor.
	*/
	DynamicArray(const DynamicArray &otherDynamicArray) NOEXCEPT
	{
		ReserveConstruct(otherDynamicArray.capacity);
		size = otherDynamicArray.size;

		MemoryUtilities::CopyMemory(array, otherDynamicArray.array, sizeof(Type) * size);
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
				array[i].~Type();
			}

			//Free the memory used by the array.
			MemoryUtilities::FreeMemory(array);
		}
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const DynamicArray &otherDynamicArray) NOEXCEPT
	{
		ReserveConstruct(otherDynamicArray.capacity);
		size = otherDynamicArray.size;

		MemoryUtilities::CopyMemory(array, otherDynamicArray.array, sizeof(Type) * size);
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
	const Type& operator[](const uint64 index) const NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	Type& operator[](const uint64 index) NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT begin() const  NOEXCEPT
	{
		return array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT begin()  NOEXCEPT
	{
		return array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT end() const NOEXCEPT
	{
		return array + size;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT end() NOEXCEPT
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
	RESTRICTED const Type *const RESTRICT Data() const NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns a pointer to the data of this dynamic array, non-const.
	*/
	RESTRICTED Type *const RESTRICT Data() NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns the back of this dynamic array, const.
	*/
	const Type& Back() const NOEXCEPT
	{
		return array[LastIndex()];
	}

	/*
	*	Returns the back of this dynamic array, non-const.
	*/
	Type& Back() NOEXCEPT
	{
		return array[LastIndex()];
	}

	/*
	*	Returns the last index if this dynamic array.
	*/
	uint64 LastIndex() const NOEXCEPT { return size - 1; }

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
			array[i].~Type();
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

		new ((void *const RESTRICT) &array[size++]) Type(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array without first checking if the array has the required capacity.
	*/
	template <class... Arguments>
	void EmplaceFast(Arguments&&... arguments) NOEXCEPT
	{
		new ((void *const RESTRICT) &array[size++]) Type(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Erases the element at the specified index in the array. Does not respect order of elements.
	*/
	void Erase(const uint64 index) NOEXCEPT
	{
		Type &object{ array[index] };

		object.~Type();
		object = std::move(Back());

		PopFast();
	}

	/*
	*	Finds and erases an element in the array. Does not respect order of elements.
	*/
	void Erase(const Type &objectToErase) NOEXCEPT
	{
		for (Type &object : *this)
		{
			if (object == objectToErase)
			{
				object.~Type();
				object = std::move(Back());
				PopFast();

				return;
			}
		}
	}

	/*
	*	Finds an element and returns it, const.
	*/
	const Optional<Type> Find(const Type &objectToFind) const NOEXCEPT
	{
		for (auto object : *this)
		{
			if (object == objectToFind)
			{
				return Optional<Type>(object);
			}
		}

		return Optional<Type>();
	}

	/*
	*	Finds an element and returns it, non-const.
	*/
	Optional<Type> Find(const Type &objectToFind) NOEXCEPT
	{
		for (auto object : *this)
		{
			if (object == objectToFind)
			{
				return Optional<Type>(object);
			}
		}

		return Optional<Type>();
	}

	/*
	*	Pops an element from the back of this dynamic array without calling the constructor first..
	*/
	void PopFast() NOEXCEPT
	{
		--size;
	}

	/*
	*	Reserves a new chunk of memory, changing the array's capacity.
	*/
	void Reserve(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		Type *const RESTRICT newArray{ static_cast<Type *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, array, sizeof(Type) * size);

		//Free the old array.
		MemoryUtilities::FreeMemory(array);

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
	}

	/*
	*	Upsizes this dynamic array, without calling the constructors of the new elements.
	*/
	void UpsizeFast(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		Type *const RESTRICT newArray{ static_cast<Type *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, array, sizeof(Type) * size);

		//Free the old array.
		MemoryUtilities::FreeMemory(array);

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
		size = newCapacity;
	}

	/*
	*	Upsizes this dynamic array, calling the constructors of the new elements.
	*/
	void UpsizeSlow(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		Type *const RESTRICT newArray{ static_cast<Type *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, array, sizeof(Type) * size);

		//Default construct the remaining objects.
		for (uint64 i = size; i < newCapacity; ++i)
		{
			new (&newArray[i]) Type;
		}

		//Free the old array.
		MemoryUtilities::FreeMemory(array);

		//Update the array and the capacity.
		array = newArray;
		capacity = newCapacity;
		size = newCapacity;
	}

private:

	//Pointer to the current array.
	Type *RESTRICT array;

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
		array = static_cast<Type *RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity));

		//Update the capacity.
		capacity = newCapacity;
	}

};