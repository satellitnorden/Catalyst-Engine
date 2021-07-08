#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/General/Vector.h>

class CatalystRandomMath final
{

public:

	/*
	*	Returns a floating point value in the range [0.0f, 1.0f].
	*/
	FORCE_INLINE static NO_DISCARD float32 RandomFloat() NOEXCEPT
	{
		static thread_local std::mt19937 random_engine{ std::random_device{}() };
		static thread_local std::uniform_real_distribution<float32> distribution{ 0.0f, 1.0f };

		return distribution(random_engine);
	}

	/*
	*	Given a range, returns a floating point value in that range.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE RandomFloatInRange(const TYPE minimum, const TYPE maximum) NOEXCEPT
	{
		static thread_local std::mt19937 random_engine{ std::random_device{}() };

		std::uniform_real_distribution<TYPE> distribution{ minimum, maximum };

		return distribution(random_engine);
	}

	/*
	*	Given a range, returns an integer value in that range.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE RandomIntegerInRange(const TYPE minimum, const TYPE maximum) NOEXCEPT
	{
		static thread_local std::mt19937 random_engine{ std::random_device{}() };

		std::uniform_int_distribution<TYPE> distribution{ minimum, maximum };

		return distribution(random_engine);
	}

	/*
	*	Template specialization of the random integer in range function.
	*/
	template <>
	FORCE_INLINE static NO_DISCARD int8 RandomIntegerInRange<int8>(const int8 minimum, const int8 maximum) NOEXCEPT
	{
		return static_cast<int8>(RandomIntegerInRange<uint32>(minimum, maximum));
	}

	/*
	*	Template specialization of the random integer in range function.
	*/
	template <>
	FORCE_INLINE static NO_DISCARD uint8 RandomIntegerInRange<uint8>(const uint8 minimum, const uint8 maximum) NOEXCEPT
	{
		return static_cast<uint8>(RandomIntegerInRange<uint32>(minimum, maximum));
	}

	/*
	*	Given a number between 0.0f and 1.0f, representing a chance in percent, return whether or not it succeeded.
	*/
	FORCE_INLINE static NO_DISCARD bool RandomChance(const float32 chance = 0.5f) NOEXCEPT
	{
		return chance > RandomFloatInRange(0.0f, 1.0f);
	}

	/*
	*	Given one or more arguments, picks a random entry and returns it.
	*/
	template <typename TYPE, typename ... LIST>
	FORCE_INLINE static NO_DISCARD TYPE RandomPick(const TYPE &first, const LIST&... arguments) NOEXCEPT
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
	*	Given a range, returns a vector with each three elements randomly generated within that range.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> RandomVector3InRange(const float32 minimum, const float32 maximum) NOEXCEPT
	{
		return Vector3<float32>(RandomFloatInRange(minimum, maximum), RandomFloatInRange(minimum, maximum), RandomFloatInRange(minimum, maximum));
	}

	/*
	*	Randomly shuffles an array.
	*/
	template <typename TYPE>
	FORCE_INLINE static void RandomShuffle(ArrayProxy<TYPE> *const RESTRICT array) NOEXCEPT
	{
		//Determine the number of iterations.
		const uint64 number_of_iterations{ array->Size() };

		for (uint64 i{ 0 }; i < number_of_iterations; ++i)
		{
			const uint64 first_index{ RandomIntegerInRange<uint64>(0, array->Size() - 1) };
			const uint64 second_index{ RandomIntegerInRange<uint64>(0, array->Size() - 1) };

			Swap(&array->At(first_index), &array->At(second_index));
		}
	}

	/*
	*	Returns a random element in the given array based on the given weights.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD TYPE& WeightedRandomElement(ArrayProxy<TYPE> &elements, ArrayProxy<float32> &weights) NOEXCEPT
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

};