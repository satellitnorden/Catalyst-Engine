#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//STL.
#include <atomic>

//For now there's no atomic class, just use the standard library one.
template <typename TYPE>
class Atomic final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE Atomic() NOEXCEPT
	{

	}

	/*
	*	Constructor taking the value as an argument.
	*/
	FORCE_INLINE Atomic(const TYPE value) NOEXCEPT
		:
		_Atomic(value)
	{

	}

	/*
	*	Performs a load operation.
	*/
	FORCE_INLINE NO_DISCARD TYPE Load() const NOEXCEPT
	{
		return _Atomic.load();
	}

	/*
	*	Performs a store operation.
	*/
	FORCE_INLINE void Store(const TYPE value) NOEXCEPT
	{
		return _Atomic.store(value);
	}

	/*
	*	Performs a compare/exchange weak operation.
	*/
	FORCE_INLINE NO_DISCARD bool CompareExchangeWeak(TYPE &expected, const TYPE desired) NOEXCEPT
	{
		return _Atomic.compare_exchange_weak(expected, desired);
	}

	/*
	*	Performs an exchange operation.
	*/
	FORCE_INLINE NO_DISCARD TYPE Exchange(const TYPE value) NOEXCEPT
	{
		return _Atomic.exchange(value);
	}

	/*
	*	Performs a fetch/add operation.
	*/
	FORCE_INLINE NO_DISCARD TYPE FetchAdd(const TYPE value) NOEXCEPT
	{
		return _Atomic.fetch_add(value);
	}

	/*
	*	Performs a fetch/sub operation.
	*/
	FORCE_INLINE NO_DISCARD TYPE FetchSub(const TYPE value) NOEXCEPT
	{
		return _Atomic.fetch_sub(value);
	}

private:

	//The underlying atomic.
	std::atomic<TYPE> _Atomic;

};