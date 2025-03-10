#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/General/Vector.h>

//STL.
#include <random>

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
	*	Returns a random point in a sphere with the given radius.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> RandomPointInSphere(const float32 radius = 1.0f) NOEXCEPT
	{
		const float32 u{ RandomFloat() };
		const float32 v{ RandomFloat() };

		const float32 theta{ u * 2.0f * BaseMathConstants::PI };
		const float32 phi{ BaseMath::ArcCosine(2.0f * v - 1.0f) };

		const float32 r{ BaseMath::CubeRoot(RandomFloat()) };

		const float32 sin_theta{ BaseMath::Sine(theta) };
		const float32 cos_theta{ BaseMath::Cosine(theta) };

		const float32 sin_phi{ BaseMath::Sine(phi) };
		const float32 cos_phi{ BaseMath::Cosine(phi) };

		const float32 X{ r * sin_phi * cos_theta };
		const float32 Y{ r * sin_phi * sin_theta };
		const float32 Z{ r * cos_phi };

		return Vector3<float32>(X, Y, Z) * radius;
	}

	/*
	*	Returns a random point on a sphere with the given radius.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float32> RandomPointOnSphere(const float32 radius = 1.0f) NOEXCEPT
	{
		const float32 theta{ RandomFloat() * BaseMathConstants::DOUBLE_PI };
		const float32 u{ 2.0f * RandomFloat() - 1.0f };
		const float32 r{ BaseMath::SquareRoot(1.0f - u * u) };

		return Vector3<float32>(r * BaseMath::Cosine(theta), r * BaseMath::Sine(theta), u) * radius;
	}

	/*
	*	Randomly shuffles an array.
	*/
	template <typename TYPE>
	FORCE_INLINE static void RandomShuffle(ArrayProxy<TYPE> *const RESTRICT array) NOEXCEPT
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

	/*
	*	Returns a random index in the given array based on the given weights.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD uint64 WeightedRandomIndex(ArrayProxy<TYPE> &elements, ArrayProxy<float32> &weights) NOEXCEPT
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

};