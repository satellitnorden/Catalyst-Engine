#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

/*
*	This class generates random, non-repeating indices for use with other systems.
*	Just a utility tool for when a randomly generated, non-repeating sequence of indices is needed.
*/
template <uint64 SIZE>
class RandomIndexer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE RandomIndexer() NOEXCEPT
	{
		//Fill in the indices.
		for (uint64 i{ 0 }; i < SIZE; ++i)
		{
			_Indices[i] = i;
		}

		//Randomly shuffle the indices.
		{
			ArrayProxy<uint64> indices_proxy{ _Indices };

			CatalystRandomMath::RandomShuffle(&indices_proxy);
		}
	}

	/*
	*	Returns the next index.
	*/
	FORCE_INLINE NO_DISCARD uint64 Next() NOEXCEPT
	{
		//Retrieve the next index.
		const uint64 next_index{ _Indices[_CurrentIndex++] };

		//Check if a reshuffle should happen.
		if (_CurrentIndex == SIZE)
		{
			//Randomly shuffle the indices.
			{
				ArrayProxy<uint64> indices_proxy{ _Indices };

				CatalystRandomMath::RandomShuffle(&indices_proxy);
			}

			//If the first index ends up being the next index, swap the first and last element so that there's no repeats. (:
			if (_Indices[0] == next_index)
			{
				Swap(&_Indices[0], &_Indices[SIZE - 1]);
			}

			//Reset the current index.
			_CurrentIndex = 0;
		}

		//Return the next index.
		return next_index;
	}

private:

	//The indices.
	StaticArray<uint64, SIZE> _Indices;

	//The current index.
	uint64 _CurrentIndex{ 0 };

};