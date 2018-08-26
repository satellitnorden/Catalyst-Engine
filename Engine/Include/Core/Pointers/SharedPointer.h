#pragma once

template <typename Type>
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
	SharedPointer(Type *const RESTRICT newPointer) RESTRICT
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
	RESTRICTED const Type *const RESTRICT operator->() const NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED Type *const RESTRICT operator->() NOEXCEPT
	{
		return pointer;
	}

private:

	//The underlying pointer.
	Type *RESTRICT pointer;

	//The reference count for this shared pointer.
	int8 *RESTRICT referenceCount;

};