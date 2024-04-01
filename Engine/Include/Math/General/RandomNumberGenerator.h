#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

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

	/*
	*	Template specialization of the random integer in range function.
	*/
	template <>
	FORCE_INLINE NO_DISCARD int8 RandomIntegerInRange<int8>(const int8 minimum, const int8 maximum) NOEXCEPT
	{
		return static_cast<int8>(RandomIntegerInRange<uint32>(minimum, maximum));
	}

	/*
	*	Template specialization of the random integer in range function.
	*/
	template <>
	FORCE_INLINE NO_DISCARD uint8 RandomIntegerInRange<uint8>(const uint8 minimum, const uint8 maximum) NOEXCEPT
	{
		return static_cast<uint8>(RandomIntegerInRange<uint32>(minimum, maximum));
	}

	/*
	*	Given a number between 0.0f and 1.0f, representing a chance in percent, return whether or not it succeeded.
	*/
	FORCE_INLINE NO_DISCARD bool RandomChance(const float32 chance = 0.5f) NOEXCEPT
	{
		return chance > RandomFloatInRange(0.0f, 1.0f);
	}

	/*
	*	Given one or more arguments, picks a random entry and returns it.
	*/
	template <typename TYPE, typename ... LIST>
	FORCE_INLINE NO_DISCARD TYPE RandomPick(const TYPE &first, const LIST&... arguments) NOEXCEPT
	{
		//Define constants.
		constexpr uint64 NUMBER_OF_ARGUMENTS{ sizeof...(LIST) + 1 };

		//Store the arguments in an array.
		TYPE array[NUMBER_OF_ARGUMENTS] = { first, arguments... };

		//Randomize the index.
		const uint64 index{ RandomIntegerInRange<uint64>(0, NUMBER_OF_ARGUMENTS - 1) };

		//Return the selected element.
		return array[index];
	}

	/*
	*	Randomly shuffles an array.
	*/
	template <typename TYPE>
	FORCE_INLINE void RandomShuffle(ArrayProxy<TYPE> *const RESTRICT array) NOEXCEPT
	{
		for (uint64 i{ 0 }; i < (array->Size() - 1); ++i)
		{
			const uint64 random_index{ RandomIntegerInRange<uint64>(i + 1, array->LastIndex()) };

			Swap(&array->At(i), &array->At(random_index));
		}
	}

	/*
	*	Returns a random element in the given array based on the given weights.
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD TYPE &WeightedRandomElement(ArrayProxy<TYPE> &elements, ArrayProxy<float32> &weights) NOEXCEPT
	{
		ASSERT(elements.Size() == weights.Size(), "Both elements and weights needs to be of the same size!");

		//Calculate the sum.
		float32 sum{ 0.0f };

		for (const float32 weight : weights)
		{
			sum += weight;
		}

		//Randomize the value.
		float32 random_value{ RandomFloatInRange(0.0f, sum) };

		//Calculate the random element.
		for (uint64 i{ 0 }, size{ elements.Size() }; i < size; ++i)
		{
			if (random_value < weights[i])
			{
				return elements[i];
			}

			else
			{
				random_value -= weights[i];
			}
		}

		//Should never get here, but might due to floating point rounding error. If so, the last element is most likely the correct one.
		return elements.Back();
	}

	/*
	*	Returns a random index in the given array based on the given weights.
	*/
	template <typename TYPE>
	FORCE_INLINE NO_DISCARD uint64 WeightedRandomIndex(ArrayProxy<TYPE> &elements, ArrayProxy<float32> &weights) NOEXCEPT
	{
		ASSERT(elements.Size() == weights.Size(), "Both elements and weights needs to be of the same size!");

		//Calculate the sum.
		float32 sum{ 0.0f };

		for (const float32 weight : weights)
		{
			sum += weight;
		}

		//Randomize the value.
		float32 random_value{ RandomFloatInRange(0.0f, sum) };

		//Calculate the random element.
		for (uint64 i{ 0 }, size{ elements.Size() }; i < size; ++i)
		{
			if (random_value < weights[i])
			{
				return i;
			}

			else
			{
				random_value -= weights[i];
			}
		}

		//Should never get here, but might due to floating point rounding error. If so, the last element is most likely the correct one.
		return elements.LastIndex();
	}

private:

	//The mersenne twister.
	std::mt19937_64 _MersenneTwister;

};