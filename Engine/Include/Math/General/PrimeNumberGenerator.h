#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>

class PrimeNumberGenerator final
{

public:

	/*
	*	Fills up the given array with prime numbers.
	*/
	FORCE_INLINE static void Generate(ArrayProxy<uint32> *const RESTRICT prime_numbers) NOEXCEPT
	{
		uint32 current{ 2 };

		uint32 number_of_generated_prime_numbers{ 0 };

		while (number_of_generated_prime_numbers < prime_numbers->Size())
		{
			bool is_prime_number{ true };

			for (uint32 i{ 0 }; i < number_of_generated_prime_numbers; ++i)
			{
				if ((current % prime_numbers->At(i)) == 0)
				{
					is_prime_number = false;

					break;
				}
			}

			if (is_prime_number)
			{
				prime_numbers->At(number_of_generated_prime_numbers++) = current;
			}

			++current;
		}
	}

};