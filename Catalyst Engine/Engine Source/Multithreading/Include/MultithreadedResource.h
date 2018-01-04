#pragma once

//Engine core.
#include <EngineCore.h>

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
		resource(arguments...)
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
	CATALYST_FORCE_INLINE const ValueType GetSafe() const CATALYST_NOEXCEPT
	{
		static bool expected = false;
		while (!lock.compare_exchange_weak(expected, true));

		const ValueType copy = resource;

		lock.store(false);

		return copy;
	}

	/*
	*	Returns the underlying resource without locking it.
	*/
	CATALYST_FORCE_INLINE const ValueType& GetUnsafe() const CATALYST_NOEXCEPT
	{
		return resource;
	}

	/*
	*	Sets the underlying resource.
	*/
	CATALYST_FORCE_INLINE void Set(const ValueType &newResource) CATALYST_NOEXCEPT
	{
		static bool expected = false;
		while (!lock.compare_exchange_weak(expected, true));

		resource = newResource;

		lock.store(false);
	}

private:

	//The underlying resource.
	ValueType resource;

	//The atomic lock.
	mutable std::atomic<bool> lock{ 0 };

};