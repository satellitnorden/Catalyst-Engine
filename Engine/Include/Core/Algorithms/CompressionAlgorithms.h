#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

namespace CompressionAlgorithms
{

	/*
	*	Compresses the given data with a run length encoding algorithm.
	*	Provides the compressed data in the 'compressed_data' variable and the new data size in the 'compressed_size' variable.
	*	Caller is responsible for calling free() on 'compressed_data' once it has been used.
	*/
	FORCE_INLINE static void CompressRunLengthEncoding(const int8 *const RESTRICT input_data, const uint64 input_size, int8 *RESTRICT *const RESTRICT compressed_data, uint64 *const RESTRICT compressed_size) NOEXCEPT
	{
		//Reset the arguments.
		*compressed_data = nullptr;
		*compressed_size = 0;

		//Absolute worst case scenario is that the compressed data is double the size of the input data, so allocate some memory to store the intermediate data.
		int8 *const RESTRICT intermediate_data{ static_cast<int8 *const RESTRICT>(Memory::AllocateMemory(input_size * 2)) };

		//Calculate all the runs.
		int8 run_value{ input_data[0] };
		int8 run_counter{ 0 };

		for (uint64 i{ 1 }; i < input_size; ++i)
		{
			//Has the run ended?
			if (run_value != input_data[i] || run_counter == INT8_MAX)
			{
				//Write how long the previous run lasted.
				intermediate_data[(*compressed_size)++] = run_counter;
				intermediate_data[(*compressed_size)++] = run_value;

				//Update the run value/counter.
				run_value = input_data[i];
				run_counter = 0;
			}

			//If not, increase the run counter!
			else
			{
				++run_counter;
			}
		}

		//Write the last run.
		intermediate_data[(*compressed_size)++] = run_counter;
		intermediate_data[(*compressed_size)++] = run_value;

		//Now that the actual compressed size is known, allocate the correct amount of memory, copy over the data and free the intermediate data.
		*compressed_data = static_cast<int8 *const RESTRICT>(malloc(*compressed_size));
		memcpy(*compressed_data, intermediate_data, *compressed_size);
		free(intermediate_data);
	}

}