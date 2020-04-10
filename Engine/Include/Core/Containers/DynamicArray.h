#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename TYPE>
class DynamicArray final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DynamicArray() NOEXCEPT
		:
		_Array(nullptr),
		_Size(0),
		_Capacity(0)
	{

	}

	/*
	*	Constructor taking an initializer list.
	*/
	FORCE_INLINE DynamicArray(const std::initializer_list<TYPE> &initializer_list) NOEXCEPT
		:
		_Size(0)
	{
		ReserveConstruct(initializer_list.size());

		for (const TYPE &element : initializer_list)
		{
			Emplace(element);
		}
	}

	/*
	*	Copy constructor.
	*/
	FORCE_INLINE DynamicArray(const DynamicArray &other) NOEXCEPT
		:
		_Array(nullptr),
		_Size(0),
		_Capacity(0)
	{
		ReserveConstruct(other._Capacity);

		for (uint64 i{ 0 }; i < other._Size; ++i)
		{
			Emplace(other._Array[i]);
		}
	}

	/*
	*	Move constructor.
	*/
	FORCE_INLINE DynamicArray(DynamicArray &&other) NOEXCEPT
		:
		_Array(nullptr),
		_Size(0),
		_Capacity(0)
	{
		_Array = other._Array;
		_Size = other._Size;
		_Capacity = other._Capacity;

		other._Array = nullptr;
		other._Size = 0;
		other._Capacity = 0;
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~DynamicArray() NOEXCEPT
	{
		Destroy();
	}

	/*
	*	Copy assignment operator overload.
	*/
	FORCE_INLINE void operator=(const DynamicArray &other) NOEXCEPT
	{
		Destroy();

		new (this) DynamicArray(other);
	}

	/*
	*	Move assignment operator overload.
	*/
	FORCE_INLINE void operator=(DynamicArray &&other) NOEXCEPT
	{
		Destroy();

		new (this) DynamicArray(std::move(other));
	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE const TYPE &operator[](const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE TYPE &operator[](const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT Begin() const  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT Begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT End() const NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT End() NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT begin() const  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT begin()  NOEXCEPT
	{
		return Begin();
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT end() const NOEXCEPT
	{
		return End();
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT end() NOEXCEPT
	{
		return End();
	}

	/*
	*	Returns the element at the specified index, const
	*/
	FORCE_INLINE const TYPE &At(const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the element at the specified index, non-const
	*/
	FORCE_INLINE TYPE &At(const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the capacity of this dynamic array.
	*/
	FORCE_INLINE uint64 Capacity() const NOEXCEPT
	{
		return _Capacity;
	}

	/*
	*	Returns the size of this dynamic array.
	*/
	FORCE_INLINE uint64 Size() const NOEXCEPT
	{
		return _Size;
	}

	/*
	*	Returns whether or not this dynamic array is empty.
	*/
	FORCE_INLINE bool Empty() const NOEXCEPT
	{
		return _Size == 0;
	}

	/*
	*	Returns a pointer to the data of this dynamic array, const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT Data() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns a pointer to the data of this dynamic array, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT Data() NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the back of this dynamic array, const.
	*/
	FORCE_INLINE const TYPE &Back() const NOEXCEPT
	{
		return _Array[LastIndex()];
	}

	/*
	*	Returns the back of this dynamic array, non-const.
	*/
	FORCE_INLINE TYPE &Back() NOEXCEPT
	{
		return _Array[LastIndex()];
	}

	/*
	*	Clears this dynamic array of elements, calling the destructor on the underlying elements.
	*/
	FORCE_INLINE void Clear() NOEXCEPT
	{
		for (uint64 i = 0; i < _Size; ++i)
		{
			_Array[i].~TYPE();
		}

		_Size = 0;
	}

	/*
	*	Destroys this dynamic array.
	*/
	FORCE_INLINE void Destroy() NOEXCEPT
	{
		//This dynamic array might have been moved from, thus we need to test the array pointer.
		if (_Array)
		{
			//Call the destructor on all objects in the array.
			for (uint64 i = 0; i < _Size; ++i)
			{
				_Array[i].~TYPE();
			}

			//Free the memory used by the array.
			Memory::Free(_Array);

			//Set it to nullptr.
			_Array = nullptr;

			//Update the size.
			_Size = 0;

			//Update the capacity.
			_Capacity = 0;
		}
	}

	/*
	*	Given constructor arguments for the object type, construct a new object at the back of the array.
	*/
	template <class... Arguments>
	FORCE_INLINE void Emplace(Arguments&&... arguments) NOEXCEPT
	{
		if (UNLIKELY(_Size >= _Capacity))
		{
			Reserve(_Size > 0 ? _Size * 2 : 2);
		}

		new ((void *const RESTRICT) &_Array[_Size++]) TYPE(std::forward<Arguments>(arguments)...);
	}

	/*
	*	Erases the element at the specified index in the array. Does not respect order of elements.
	*/
	FORCE_INLINE void EraseAt(const uint64 index) NOEXCEPT
	{
		TYPE &object{ _Array[index] };

		object.~TYPE();
		object = std::move(Back());

		--_Size;
	}

	/*
	*	Finds and erases an element in the array. Does not respect order of elements.
	*/
	FORCE_INLINE void Erase(const TYPE &element) NOEXCEPT
	{
		for (TYPE &object : *this)
		{
			if (object == element)
			{
				object.~TYPE();
				object = std::move(Back());
				
				--_Size;

				return;
			}
		}
	}

	/*
	*	Returns a (linearly) interpolated element, float32 overload.
	*/
	FORCE_INLINE TYPE Interpolate(const float32 index) const NOEXCEPT
	{
		const uint64 first_index{ static_cast<uint64>(index) };
		const uint64 second_index{ first_index < LastIndex() ? first_index + 1 : first_index };
		const float32 alpha{ index - static_cast<float32>(first_index) };

		const TYPE first_element{ At(first_index) };
		const TYPE Second_element{ At(second_index) };

		return (first_element * (1.0f - alpha)) + (Second_element * alpha);
	}

	/*
	*	Returns a (linearly) interpolated element, float64 overload.
	*/
	FORCE_INLINE TYPE Interpolate(const float64 index) const NOEXCEPT
	{
		const uint64 first_index{ static_cast<uint64>(index) };
		const uint64 second_index{ first_index < LastIndex() ? first_index + 1 : first_index };
		const float64 alpha{ index - static_cast<float64>(first_index) };

		const TYPE first_element{ At(first_index) };
		const TYPE second_element{ At(second_index) };

		return (first_element * (1.0 - alpha)) + (second_element * alpha);
	}

	/*
	*	Returns the last index if this dynamic array.
	*/
	FORCE_INLINE uint64 LastIndex() const NOEXCEPT
	{
		return _Size - 1;
	}

	/*
	*	Pops an element from the back of this dynamic array.
	*/
	FORCE_INLINE void Pop() NOEXCEPT
	{
		_Array[LastIndex()].~TYPE();

		--_Size;
	}

	/*
	*	Reserves a new chunk of memory, changing the array's capacity.
	*/
	FORCE_INLINE void Reserve(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		TYPE *const RESTRICT newArray{ static_cast<TYPE *const RESTRICT>(Memory::Allocate(sizeof(TYPE) * newCapacity)) };

		//Move over all objects from the old array to the new array.
		Memory::Copy(newArray, _Array, sizeof(TYPE) * _Size);

		//Free the old array.
		Memory::Free(_Array);

		//Update the array and the capacity.
		_Array = newArray;
		_Capacity = newCapacity;
	}

	/*
	*	Upsizes this dynamic array.
	*/
	template <bool CONSTRUCT>
	FORCE_INLINE void Upsize(const uint64 new_capacity) NOEXCEPT
	{
		ASSERT(false, "Intentionally unimplemented!");
	}

	/*
	*	Upsizes this dynamic array.
	*/
	template<>
	FORCE_INLINE void Upsize<false>(const uint64 new_capacity) NOEXCEPT
	{
		//Allocate the new array.
		TYPE *const RESTRICT new_array{ static_cast<TYPE *const RESTRICT>(Memory::Allocate(sizeof(TYPE) * new_capacity)) };

		//Move over all objects from the old array to the new array.
		Memory::Copy(new_array, _Array, sizeof(TYPE) * _Size);

		//Free the old array.
		Memory::Free(_Array);

		//Update the array and the capacity.
		_Array = new_array;
		_Capacity = new_capacity;
		_Size = new_capacity;
	}

	/*
	*	Upsizes this dynamic array.
	*/
	template<>
	FORCE_INLINE void Upsize<true>(const uint64 new_capacity) NOEXCEPT
	{
		//Allocate the new array.
		TYPE *const RESTRICT new_array{ static_cast<TYPE *const RESTRICT>(Memory::Allocate(sizeof(TYPE) * new_capacity)) };

		//Move over all objects from the old array to the new array.
		Memory::Copy(new_array, _Array, sizeof(TYPE) * _Size);

		//Default construct the remaining objects.
		for (uint64 i{ _Size }; i < new_capacity; ++i)
		{
			new (&new_array[i]) TYPE;
		}

		//Free the old array.
		Memory::Free(_Array);

		//Update the array and the capacity.
		_Array = new_array;
		_Capacity = new_capacity;
		_Size = new_capacity;
	}

private:

	//Pointer to the current array.
	TYPE *RESTRICT _Array;

	//The current size of this dynamic array.
	uint64 _Size;

	//The current capacity of this dynamic array.
	uint64 _Capacity;

	/*
	*	Reserves a new chunk of memory, changing the array's capacity, without copying over the old array.
	*/
	FORCE_INLINE void ReserveConstruct(const uint64 newCapacity) NOEXCEPT
	{
		//Allocate the new array.
		_Array = static_cast<TYPE *RESTRICT>(Memory::Allocate(sizeof(TYPE) * newCapacity));

		//Update the capacity.
		_Capacity = newCapacity;
	}

};