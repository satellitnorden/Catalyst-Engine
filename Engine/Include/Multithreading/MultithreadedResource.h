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
		resource(std::forward<Arguments>(arguments)...)
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
		ScopedLock<Spinlock> scopedLock{ lock };

		return resource;
	}

	/*
	*	Returns the underlying resource without locking it.
	*/
	const Type& GetUnsafe() const NOEXCEPT
	{
		return resource;
	}

	/*
	*	Sets the underlying resource.
	*/
	void Set(const Type &newResource) NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ lock };

		resource = newResource;
	}

private:

	//The underlying resource.
	Type resource;

	//The spin lock.
	mutable Spinlock lock;

};