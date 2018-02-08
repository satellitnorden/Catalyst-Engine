#pragma once

template <class ObjectType, size_t ArraySize>
class StaticArray final
{

public:

	/*
	*	Default constructor.
	*/
	StaticArray() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Default destructor.
	*/
	~StaticArray() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	const ObjectType& operator[](const size_t index) const CATALYST_NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	ObjectType& operator[](const size_t index) CATALYST_NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	CATALYST_RESTRICTED const ObjectType* begin() const CATALYST_NOEXCEPT
	{
		return array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* begin()  CATALYST_NOEXCEPT
	{
		return array;
	}

	/*
	*	End iterator, const.
	*/
	CATALYST_RESTRICTED const ObjectType* end() const CATALYST_NOEXCEPT
	{
		return array + ArraySize;
	}

	/*
	*	End iterator, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* end() CATALYST_NOEXCEPT
	{
		return array + ArraySize;
	}

	/*
	*	Returns a pointer to the data of the array, const.
	*/
	CATALYST_RESTRICTED const ObjectType* const Data() const CATALYST_NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns a pointer to the data of the array, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* Data() CATALYST_NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns the size of this array.
	*/
	size_t Size() const CATALYST_NOEXCEPT
	{
		return ArraySize;
	}

private:

	//The underlying array.
	ObjectType array[ArraySize];

};