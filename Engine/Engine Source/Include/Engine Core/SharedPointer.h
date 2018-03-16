#pragma once

template <class ObjectType>
class SharedPointer final
{

public:

	/*
	*	Default constructor.
	*/
	SharedPointer() NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	SharedPointer(const SharedPointer &otherPointer) NOEXCEPT
		:
		pointer(otherPointer.pointer),
		referenceCount(otherPointer.referenceCount)
	{
		++(*referenceCount);
	}

	/*
	*	Move constructor.
	*/
	SharedPointer(SharedPointer &&otherPointer) NOEXCEPT
		:
		pointer(otherPointer.pointer),
		referenceCount(otherPointer.referenceCount)
	{
		++(*referenceCount);
	}

	/*
	*	Constructor taking a raw pointer as it's argument.
	*/
	SharedPointer(ObjectType *const RESTRICT newPointer) RESTRICT
		:
		pointer(newPointer),
		referenceCount(new int8)
	{
		++(*referenceCount);
	}

	/*
	*	Default destructor.
	*/
	~SharedPointer() NOEXCEPT
	{
		//Decrement the reference count and delete the pointer if it's reached 0.
		if (--(*referenceCount) == 0)
		{
			delete referenceCount;
			delete pointer;
		}
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const SharedPointer &otherPointer) NOEXCEPT
	{
		pointer = otherPointer.pointer;
		referenceCount = otherPointer.referenceCount;

		++(*referenceCount);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(SharedPointer &&otherPointer) NOEXCEPT
	{
		pointer = otherPointer.pointer;
		referenceCount = otherPointer.referenceCount;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED const ObjectType* const operator->() const NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED ObjectType* operator->() NOEXCEPT
	{
		return pointer;
	}

private:

	//The underlying pointer.
	ObjectType *RESTRICT pointer;

	//The reference count for this shared pointer.
	int8 *RESTRICT referenceCount;

};