#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
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
		_Array(nullptr),
		_Size(0),
		_Capacity(0)
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
		MemoryUtilities::CopyMemory(_Array, initializerList.begin(), sizeof(Type) * _Capacity);

		//Set the size equal to the capacity.
		_Size = _Capacity;
	}

	/*
	*	Copy constructor.
	*/
	DynamicArray(const DynamicArray &otherDynamicArray) NOEXCEPT
	{
		ReserveConstruct(otherDynamicArray._Capacity);
		_Size = otherDynamicArray._Size;

		MemoryUtilities::CopyMemory(_Array, otherDynamicArray._Array, sizeof(Type) * _Size);
	}

	/*
	*	Move constructor.
	*/
	DynamicArray(DynamicArray &&otherDynamicArray) NOEXCEPT
	{
		_Array = otherDynamicArray._Array;
		_Size = otherDynamicArray._Size;
		_Capacity = otherDynamicArray._Capacity;

		otherDynamicArray._Array = nullptr;
	}

	/*
	*	Default destructor.
	*/
	~DynamicArray() NOEXCEPT
	{
		//This dynamic array might have been moved from, thus we need to test the array pointer.
		DestroyArray();
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const DynamicArray &otherDynamicArray) NOEXCEPT
	{
		DestroyArray();

		ReserveConstruct(otherDynamicArray._Capacity);
		_Size = otherDynamicArray._Size;

		MemoryUtilities::CopyMemory(_Array, otherDynamicArray._Array, sizeof(Type) * _Size);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(DynamicArray &&otherDynamicArray) NOEXCEPT
	{
		DestroyArray();

		_Array = otherDynamicArray._Array;
		_Size = otherDynamicArray._Size;
		_Capacity = otherDynamicArray._Capacity;

		otherDynamicArray._Array = nullptr;
	}

	/*
	*	Subscript operator overload, const.
	*/
	const Type& operator[](const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	Type& operator[](const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT Begin() const  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT Begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT End() const NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT End() NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT begin() const  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT begin()  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const Type *const RESTRICT end() const NOEXCEPT
	{
		return End();
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED Type *const RESTRICT end() NOEXCEPT
	{
		return End();
	}

	/*
	*	Returns the element at the specified index, const
	*/
	const Type& At(const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the element at the specified index, non-const
	*/
	Type& At(const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the capacity of this dynamic array.
	*/
	uint64 Capacity() const NOEXCEPT { return _Capacity; }

	/*
	*	Returns the size of this dynamic array.
	*/
	uint64 Size() const NOEXCEPT { return _Size; }

	/*
	*	Returns whether or not this dynamic array is empty.
	*/
	bool Empty() const NOEXCEPT { return _Size == 0; }

	/*
	*	Returns a pointer to the data of this dynamic array, const.
	*/
	RESTRICTED const Type *const RESTRICT Data() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns a pointer to the data of this dynamic array, non-const.
	*/
	RESTRICTED Type *const RESTRICT Data() NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the back of this dynamic array, const.
	*/
	const Type& Back() const NOEXCEPT
	{
		return _Array[LastIndex()];
	}

	/*
	*	Returns the back of this dynamic array, non-const.
	*/
	Type& Back() NOEXCEPT
	{
		return _Array[LastIndex()];
	}

	/*
	*	Returns the last index if this dynamic array.
	*/
	uint64 LastIndex() const NOEXCEPT { return _Size - 1; }

	/*
	*	Clears this dynamic array of elements without calling the destructor on the underlying elements.
	*/
	void ClearFast() NOEXCEPT
	{
		_Size = 0;
	}

	/*
	*	Clears this dynamic array of elements, calling the destructor on the underlying elements.
	*/
	void ClearSlow() NOEXCEPT
	{
		for (uint64 i = 0; i < _Size; ++i)
		{
			_Array[i].~Type();
		}

		_Size = 0;
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array.
	*/
	template <class... Arguments>
	void EmplaceSlow(Arguments&&... arguments) NOEXCEPT
	{
		if (UNLIKELY(_Size >= _Capacity))
		{
			Reserve(_Size > 0 ? _Size * 3 : 16);
		}

		new ((void *const RESTRICT) &_Array[_Size++]) Type(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array without first checking if the array has the required capacity.
	*/
	template <class... Arguments>
	void EmplaceFast(Arguments&&... arguments) NOEXCEPT
	{
		new ((void *const RESTRICT) &_Array[_Size++]) Type(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Erases the element at the specified index in the array. Does not respect order of elements.
	*/
	void EraseAt(const uint64 index) NOEXCEPT
	{
		Type &object{ _Array[index] };

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
	*	Pops an element from the back of this dynamic array without calling the constructor first..
	*/
	void PopFast() NOEXCEPT
	{
		--_Size;
	}

	/*
	*	Reserves a new chunk of memory, changing the array's capacity.
	*/
	void Reserve(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		Type *const RESTRICT newArray{ static_cast<Type *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, _Array, sizeof(Type) * _Size);

		//Free the old array.
		MemoryUtilities::FreeMemory(_Array);

		//Update the array and the capacity.
		_Array = newArray;
		_Capacity = newCapacity;
	}

	/*
	*	Upsizes this dynamic array, without calling the constructors of the new elements.
	*/
	void UpsizeFast(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		Type *const RESTRICT newArray{ static_cast<Type *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, _Array, sizeof(Type) * _Size);

		//Free the old array.
		MemoryUtilities::FreeMemory(_Array);

		//Update the array and the capacity.
		_Array = newArray;
		_Capacity = newCapacity;
		_Size = newCapacity;
	}

	/*
	*	Upsizes this dynamic array, calling the constructors of the new elements.
	*/
	void UpsizeSlow(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		Type *const RESTRICT newArray{ static_cast<Type *const RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		MemoryUtilities::CopyMemory(newArray, _Array, sizeof(Type) * _Size);

		//Default construct the remaining objects.
		for (uint64 i = _Size; i < newCapacity; ++i)
		{
			new (&newArray[i]) Type;
		}

		//Free the old array.
		MemoryUtilities::FreeMemory(_Array);

		//Update the array and the capacity.
		_Array = newArray;
		_Capacity = newCapacity;
		_Size = newCapacity;
	}

private:

	//Pointer to the current array.
	Type *RESTRICT _Array;

	//The current size of this dynamic array.
	uint64 _Size;

	//The current capacity of this dynamic array.
	uint64 _Capacity;

	/*
	*	Destroys the underlying array.
	*/
	void DestroyArray() NOEXCEPT
	{
		if (_Array)
		{
			//Call the destructor on all objects in the array.
			for (uint64 i = 0; i < _Size; ++i)
			{
				_Array[i].~Type();
			}

			//Free the memory used by the array.
			MemoryUtilities::FreeMemory(_Array);

			//Set it to nullptr.
			_Array = nullptr;
		}
	}

	/*
	*	Reserves a new chunk of memory, changing the array's capacity, without copying over the old array.
	*/
	void ReserveConstruct(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		_Array = static_cast<Type *RESTRICT>(MemoryUtilities::AllocateMemory(sizeof(Type) * newCapacity));

		//Update the capacity.
		_Capacity = newCapacity;
	}

};