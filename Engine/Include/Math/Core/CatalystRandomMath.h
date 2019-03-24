#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class CatalystRandomMath final
{

public:

	/*
	*	Given a range, returns a floating point value in that range.
	*/
	FORCE_INLINE static NO_DISCARD float RandomFloatInRange(const float minimum, const float maximum) NOEXCEPT
	{
		static thread_local std::random_device randomDevice;
		static thread_local std::mt19937 randomEngine{ randomDevice() };

		std::uniform_real_distribution<float> distribution{ minimum, maximum };

		return distribution(randomEngine);
	}

	/*
	*	Given a range, returns an integer value in that range.
	*/
	template <typename Type>
	FORCE_INLINE static NO_DISCARD Type RandomIntegerInRange(const Type minimum, const Type maximum) NOEXCEPT
	{
		static thread_local std::random_device randomDevice;
		static thread_local std::mt19937 randomEngine{ randomDevice() };

		std::uniform_int_distribution<Type> distribution{ minimum, maximum };

		return distribution(randomEngine);
	}

	/*
	*	Given a number between 0.0f and 1.0f, representing a chance in percent, return whether or not it succeeded.
	*/
	FORCE_INLINE static NO_DISCARD bool RandomChance(const float chance) NOEXCEPT
	{
		return chance > RandomFloatInRange(0.0f, 1.0f);
	}

	/*
	*	Given a range, returns a vector with each three elements randomly generated within that range.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float> RandomVector3InRange(const float minimum, const float maximum) NOEXCEPT
	{
		return Vector3<float>(RandomFloatInRange(minimum, maximum), RandomFloatInRange(minimum, maximum), RandomFloatInRange(minimum, maximum));
	}

};