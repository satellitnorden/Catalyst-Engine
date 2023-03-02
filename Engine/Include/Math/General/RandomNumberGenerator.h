#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class RandomNumberGenerator final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE RandomNumberGenerator() NOEXCEPT
		:
		_MersenneTwister(std::random_device{}())
	{

	}

	/*
	*	Constructor taking a seed.
	*/
	FORCE_INLINE RandomNumberGenerator(const uint64 seed) NOEXCEPT
		:
		_MersenneTwister(seed)
	{

	}

	/*
	*	Sets the seed.
	*/
	FORCE_INLINE void SetSeed(const uint64 seed) NOEXCEPT
	{
		_MersenneTwister.seed(seed);
	}

	/*
	*	Generates a random float in the range [0.0f, 1.0f].
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD TYPE RandomFloat() NOEXCEPT
	{
		static std::uniform_real_distribution<TYPE> DISTRIBUTION{ static_cast<TYPE>(0), static_cast<TYPE>(1) };

		return DISTRIBUTION(_MersenneTwister);
	}

	/*
	*	Generates a random float in the given range.
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD TYPE RandomFloatInRange(const TYPE minimum, const TYPE maximum) NOEXCEPT
	{
		std::uniform_real_distribution<TYPE> DISTRIBUTION{ minimum, maximum };

		return DISTRIBUTION(_MersenneTwister);
	}

	/*
	*	Generates a random integer in the given range.
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD TYPE RandomIntegerInRange(const TYPE minimum, const TYPE maximum) NOEXCEPT
	{
		std::uniform_int_distribution<TYPE> DISTRIBUTION{ minimum, maximum };

		return DISTRIBUTION(_MersenneTwister);
	}

private:

	//The mersenne twister.
	std::mt19937_64 _MersenneTwister;

};