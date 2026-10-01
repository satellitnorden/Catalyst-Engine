#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

/*
*	This class generates random, non-repeating indices for use with other systems.
*	Just a utility tool for when a randomly generated, non-repeating sequence of indices is needed.
*	Comes in two flavors - a static variant for when the number of indices is known at compile time and a dynamic variant for when it isn't.
*/
template <uint64 SIZE>
class StaticRandomIndexer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE StaticRandomIndexer() NOEXCEPT
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

class DynamicRandomIndexer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DynamicRandomIndexer() NOEXCEPT
	{
		
	}

	/*
	*	Initializes this dynamic random indexer.
	*/
	FORCE_INLINE void Initialize(const uint64 size) NOEXCEPT
	{
		//Upsize the indices.
		_Indices.Upsize<false>(size);

		//Fill in the indices.
		for (uint64 i{ 0 }; i < size; ++i)
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
		if (_CurrentIndex == _Indices.Size())
		{
			//Randomly shuffle the indices.
			{
				ArrayProxy<uint64> indices_proxy{ _Indices };

				CatalystRandomMath::RandomShuffle(&indices_proxy);
			}

			//If the first index ends up being the next index, swap the first and last element so that there's no repeats. (:
			if (_Indices[0] == next_index)
			{
				Swap(&_Indices[0], &_Indices[_Indices.LastIndex()]);
			}

			//Reset the current index.
			_CurrentIndex = 0;
		}

		//Return the next index.
		return next_index;
	}

private:

	//The indices.
	DynamicArray<uint64> _Indices;

	//The current index.
	uint64 _CurrentIndex{ 0 };

};