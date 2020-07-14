#pragma once

//Math.
#include <Math/Core/CatalystBaseMath.h>

namespace CompressionAlgorithms
{

	namespace Catalyst
	{

		/*
		*	Compresses the given data with the Catalyst algorithm.
		*	General purpose in-place algorithm.
		*/
		FORCE_INLINE static void Compress(uint8* const RESTRICT input_data, const uint64 input_size, uint64* const RESTRICT compressed_size) NOEXCEPT
		{
			//Define constants.
			constexpr uint8 MAXIMUM_LENGTH{ 127 }; //Maximum value for a single byte with 7 bits available.

			/*
			*	First pass, encode a run length encoding-type compression with the following layout;
			*	First byte has high bit (BIT(7)) set if the following byte are run length encoded,
			*	otherwise the following byte(s) are literal bytes.
			*	The remaining 7 bits in the first byte denotes the length.
			*	If the high bit is set, the decoding step should insert N bytes of the following bytes to the decoded data,
			*	otherwise it should insert the following N bytes as literal bytes.
			*/
			uint64 current_write_index{ 0 };

			int8 current_mode{ -1 }; //-1 == undecided, 0 == literal bytes, 1 == run length bytes.

			uint8 current_value{ input_data[0] };
			uint8 current_length{ 1 };

			for (uint64 i{ 1 }; i < input_size; ++i)
			{
				switch (current_mode)
				{
					//Undecided.
					case -1:
					{
						//If the current mode is undecided, look at the current value and compare it to previous value to determine if this should be a literal bytes run or a run length run.
						current_mode = current_value == input_data[i] ? 1 : 0;

						++current_length;

						break;
					}

					//Literal bytes.
					case 0:
					{
						//If the current value and the previous value are the same, the literal bytes should be written to the buffer and a run length run should be initiated.
						if (current_value == input_data[i] || current_length == MAXIMUM_LENGTH || i == (input_size - 1))
						{
							input_data[current_write_index] = current_length;

							for (uint8 j{ 0 }; j < current_length; ++j)
							{
								input_data[current_write_index + j + 1] = input_data[i - current_length + j];
							}

							current_write_index += 1 + current_length;

							current_mode = 1;
						
							current_value = input_data[i];
							current_length = 1;
						}

						else
						{
							++current_length;
						}

						break;
					}

					//Run length.
					case 1:
					{
						//If the current value and the previous value are not the same, the literal bytes should be written to the buffer and a literal bytes run should be initiated.
						if (current_value != input_data[i] || current_length == MAXIMUM_LENGTH || i == (input_size - 1))
						{
							input_data[current_write_index] = current_length;
							SET_BIT(input_data[current_write_index], BIT(7));

							input_data[current_write_index + 1] = current_value;

							current_write_index += 2;

							current_mode = 0;

							current_value = input_data[i];
							current_length = 1;
						}

						else
						{
							++current_length;
						}

						break;
					}
				}
			}

			//Write the compressed size.
			*compressed_size = current_write_index;

			ASSERT(input_size > *compressed_size, "Well, this didn't turn it how it was supposed to. /:");
		}

	}

}