#pragma once

template <class ObjectType>
class SharedPointer final
{

public:

	/*
	*	Default constructor.
	*/
	SharedPointer() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Copy constructor.
	*/
	SharedPointer(const SharedPointer &otherPointer) CATALYST_NOEXCEPT
		:
		pointer(otherPointer.pointer),
		referenceCount(otherPointer.referenceCount)
	{
		++(*referenceCount);
	}

	/*
	*	Move constructor.
	*/
	SharedPointer(SharedPointer &&otherPointer) CATALYST_NOEXCEPT
		:
		pointer(otherPointer.pointer),
		referenceCount(otherPointer.referenceCount)
	{
		++(*referenceCount);
	}

	/*
	*	Constructor taking a raw pointer as it's argument.
	*/
	SharedPointer(ObjectType *const CATALYST_RESTRICT newPointer) CATALYST_RESTRICT
		:
		pointer(newPointer),
		referenceCount(new int8)
	{
		++(*referenceCount);
	}

	/*
	*	Default destructor.
	*/
	~SharedPointer() CATALYST_NOEXCEPT
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
	void operator=(const SharedPointer &otherPointer) CATALYST_NOEXCEPT
	{
		pointer = otherPointer.pointer;
		referenceCount = otherPointer.referenceCount;

		++(*referenceCount);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(SharedPointer &&otherPointer) CATALYST_NOEXCEPT
	{
		pointer = otherPointer.pointer;
		referenceCount = otherPointer.referenceCount;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_RESTRICTED const ObjectType* const operator->() const CATALYST_NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* operator->() CATALYST_NOEXCEPT
	{
		return pointer;
	}

private:

	//The underlying pointer.
	ObjectType *CATALYST_RESTRICT pointer;

	//The reference count for this shared pointer.
	int8 *CATALYST_RESTRICT referenceCount;

};