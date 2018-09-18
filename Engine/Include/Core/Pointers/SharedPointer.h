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
		_Pointer(otherPointer.pointer),
		_ReferenceCount(otherPointer._ReferenceCount)
	{
		++(*_ReferenceCount);
	}

	/*
	*	Move constructor.
	*/
	SharedPointer(SharedPointer &&otherPointer) NOEXCEPT
		:
		_Pointer(otherPointer.pointer),
		_ReferenceCount(otherPointer._ReferenceCount)
	{
		++(*_ReferenceCount);
	}

	/*
	*	Constructor taking a raw pointer as it's argument.
	*/
	SharedPointer(Type *const RESTRICT newPointer) RESTRICT
		:
		_Pointer(newPointer),
		_ReferenceCount(new (MemoryUtilities::ThreadSafePoolAllocate<sizeof(int8)>()) int8)
	{
		++(*_ReferenceCount);
	}

	/*
	*	Default destructor.
	*/
	~SharedPointer() NOEXCEPT
	{
		//Decrement the reference count and delete the pointer if it's reached 0.
		if (--(*_ReferenceCount) == 0)
		{
			MemoryUtilities::ThreadSafePoolDeAllocate<sizeof(int8)>(_ReferenceCount);
			delete _Pointer;
		}
	}

	/*
	*	Copy assignment operator overload.
	*/
	void operator=(const SharedPointer &otherPointer) NOEXCEPT
	{
		_Pointer = otherPointer._Pointer;
		_ReferenceCount = otherPointer._ReferenceCount;

		++(*_ReferenceCount);
	}

	/*
	*	Move assignment operator overload.
	*/
	void operator=(SharedPointer &&otherPointer) NOEXCEPT
	{
		_Pointer = otherPointer._Pointer;
		_ReferenceCount = otherPointer._ReferenceCount;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED const Type *const RESTRICT operator->() const NOEXCEPT
	{
		return _Pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED Type *const RESTRICT operator->() NOEXCEPT
	{
		return _Pointer;
	}

private:

	//The underlying pointer.
	Type *RESTRICT _Pointer;

	//The reference count for this shared pointer.
	int8 *RESTRICT _ReferenceCount;

};