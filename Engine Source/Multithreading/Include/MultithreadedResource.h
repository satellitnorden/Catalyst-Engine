#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <ScopedLock.h>
#include <SpinLock.h>

template <class ResourceType>
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
	MultithreadedResource(Arguments&&... arguments) CATALYST_NOEXCEPT
		:
		resource(std::forward<Arguments>(arguments)...)
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
	const ResourceType GetSafe() const CATALYST_NOEXCEPT
	{
		ScopedLock<Spinlock>{ spinlock };

		return resource;
	}

	/*
	*	Returns the underlying resource without locking it.
	*/
	const ResourceType& GetUnsafe() const CATALYST_NOEXCEPT
	{
		return resource;
	}

	/*
	*	Sets the underlying resource.
	*/
	void Set(const ResourceType &newResource) CATALYST_NOEXCEPT
	{
		ScopedLock<Spinlock>{ spinlock };

		resource = newResource;
	}

private:

	//The underlying resource.
	ResourceType resource;

	//The spin lock.
	mutable Spinlock spinlock;

};