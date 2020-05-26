#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/Pair.h>

template <typename KEY_TYPE, typename VALUE_TYPE>
class HashTable final
{

public:

	/*
	*	Const iterator class definition.
	*/
	template <typename TYPE>
	class ConstIterator final
	{

	public:

		/*
		*	Constructor taking the elements as argument.
		*/
		FORCE_INLINE ConstIterator(const DynamicArray<TYPE> &initial_elements) NOEXCEPT
			:
			_Elements(initial_elements)
		{

		}

		/*
		*	Begin iterator.
		*/
		FORCE_INLINE RESTRICTED const TYPE *const RESTRICT begin() NOEXCEPT
		{
			return _Elements.Begin();
		}

		/*
		*	End iterator.
		*/
		FORCE_INLINE RESTRICTED const TYPE *const RESTRICT end() NOEXCEPT
		{
			return _Elements.End();
		}

	private:

		//The elements.
		const DynamicArray<TYPE> &_Elements;

	};

	/*
	*	Iterator class definition.
	*/
	template <typename TYPE>
	class Iterator final
	{

	public:

		/*
		*	Constructor taking the elements as argument.
		*/
		FORCE_INLINE Iterator(DynamicArray<TYPE> &initial_elements) NOEXCEPT
			:
			_Elements(initial_elements)
		{

		}

		/*
		*	Begin iterator.
		*/
		FORCE_INLINE RESTRICTED TYPE *const RESTRICT begin() NOEXCEPT
		{
			return _Elements.Begin();
		}

		/*
		*	End iterator.
		*/
		FORCE_INLINE RESTRICTED TYPE *const RESTRICT end() NOEXCEPT
		{
			return _Elements.End();
		}
	
	private:

		//The elements.
		DynamicArray<TYPE> &_Elements;

	};

	/*
	*	Returns a key iterator, const.
	*/
	FORCE_INLINE NO_DISCARD ConstIterator<KEY_TYPE> KeyIterator() const NOEXCEPT
	{
		return ConstIterator<KEY_TYPE>(_Keys);
	}

	/*
	*	Returns a key iterator, non-const.
	*/
	FORCE_INLINE NO_DISCARD Iterator<KEY_TYPE> KeyIterator() NOEXCEPT
	{
		return Iterator<KEY_TYPE>(_Keys);
	}

	/*
	*	Returns a value iterator, const.
	*/
	FORCE_INLINE NO_DISCARD ConstIterator<VALUE_TYPE> ValueIterator() const NOEXCEPT
	{
		return ConstIterator<VALUE_TYPE>(_Values);
	}

	/*
	*	Returns a value iterator, non-const.
	*/
	FORCE_INLINE NO_DISCARD Iterator<VALUE_TYPE> ValueIterator() NOEXCEPT
	{
		return Iterator<VALUE_TYPE>(_Values);
	}

	/*
	*	Adds the given key/value pair.
	*/
	FORCE_INLINE void Add(const KEY_TYPE &key, const VALUE_TYPE &value) NOEXCEPT
	{
		_Keys.Emplace(key);
		_Values.Emplace(value);
	}

	/*
	*	Finds the value with the given key and returns a pointer to it, or nullptr if none was found.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD VALUE_TYPE *const RESTRICT Find(const KEY_TYPE &key) NOEXCEPT
	{
		for (uint64 i{ 0 }, size{ _Keys.Size() }; i < size; ++i)
		{
			if (_Keys[i] == key)
			{
				return &_Values[i];
			}
		}

		return nullptr;
	}

private:

	//The underlying keys.
	DynamicArray<KEY_TYPE> _Keys;

	//The underlying values.
	DynamicArray<VALUE_TYPE> _Values;

};