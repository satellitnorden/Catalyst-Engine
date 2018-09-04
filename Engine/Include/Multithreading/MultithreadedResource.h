#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>
#include <Multithreading/SpinLock.h>

template <typename Type>
class MultithreadedResource final
{

public:

	/*
	*	Default constructor.
	*/
	MultithreadedResource() NOEXCEPT
	{

	}

	/*
	*	Constructor forwarding constructor arguments to the underlying resource.
	*/
	template<class... Arguments>
	MultithreadedResource(Arguments&&... arguments) NOEXCEPT
		:
		_Resource(std::forward<Arguments>(arguments)...)
	{

	}

	/*
	*	Default destructor.
	*/
	~MultithreadedResource() NOEXCEPT
	{

	}

	/*
	*	Returns the underlying resource.
	*/
	const Type GetSafe() const NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		return _Resource;
	}

	/*
	*	Returns the underlying resource without locking it.
	*/
	const Type& GetUnsafe() const NOEXCEPT
	{
		return _Resource;
	}

	/*
	*	Sets the underlying resource.
	*/
	void Set(const Type &newResource) NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ _Lock };

		_Resource = newResource;
	}

private:

	//The underlying resource.
	Type _Resource;

	//The spin lock.
	mutable Spinlock _Lock;

};