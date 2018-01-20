#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <ScopedLock.h>
#include <SpinLock.h>

template <class ValueType>
class MultithreadedResource
{

public:

	/*
	*	Default constructor.
	*/
	MultithreadedResource() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Constructor forwarding constructor arguments to the underlying resource.
	*/
	template<class... Arguments>
	MultithreadedResource(Arguments... arguments) CATALYST_NOEXCEPT
		:
		resource(std::forward(arguments)...)
	{

	}

	/*
	*	Default destructor.
	*/
	~MultithreadedResource() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Returns the underlying resource.
	*/
	const ValueType GetSafe() const CATALYST_NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ spinlock };

		return resource;
	}

	/*
	*	Returns the underlying resource without locking it.
	*/
	const ValueType& GetUnsafe() const CATALYST_NOEXCEPT
	{
		return resource;
	}

	/*
	*	Sets the underlying resource.
	*/
	void Set(const ValueType &newResource) CATALYST_NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ spinlock };

		resource = newResource;
	}

private:

	//The underlying resource.
	ValueType resource;

	//The spin lock.
	mutable Spinlock spinlock;

};