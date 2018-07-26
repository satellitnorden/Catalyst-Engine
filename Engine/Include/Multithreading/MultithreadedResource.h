#pragma once

//Core.
#include <Core/EngineCore.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>
#include <Multithreading/SpinLock.h>

template <class ResourceType>
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
	const ResourceType GetSafe() const NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ lock };

		return resource;
	}

	/*
	*	Returns the underlying resource without locking it.
	*/
	const ResourceType& GetUnsafe() const NOEXCEPT
	{
		return resource;
	}

	/*
	*	Sets the underlying resource.
	*/
	void Set(const ResourceType &newResource) NOEXCEPT
	{
		ScopedLock<Spinlock> scopedLock{ lock };

		resource = newResource;
	}

private:

	//The underlying resource.
	ResourceType resource;

	//The spin lock.
	mutable Spinlock lock;

};