#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

namespace Audio
{

	//TODO: This should probably be it's own class. (:
	namespace Buffer
	{

		/*
		*	Returns if the given buffer is a mono.
		*/
		FORCE_INLINE NO_DISCARD bool IsMono(const DynamicArray<DynamicArray<float32>> &buffer, const bool *const RESTRICT previous_value = nullptr) NOEXCEPT
		{
			//Define constants.
			constexpr float32 THRESHOLD{ FLOAT32_EPSILON };

			//If the buffer is already mono, then it's mono. (:
			if (buffer.Size() == 1)
			{
				return true;
			}

			/*
			*	This function is only built for detecting if stereo buffers is actually mono,
			*	so if we have a larger buffer than that, just return false.
			*/
			if (buffer.Size() > 2)
			{
				return false;
			}

			//Cache the number of samples.
			const uint32 number_of_samples{ static_cast<uint32>(buffer.At(0).Size()) };

			//Iterate over all samples, calculating their differences and see if it passes the threshold.
			float32 total_sum{ 0.0f };

			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Calculate the difference.
				const float32 difference{ BaseMath::Absolute(buffer.At(0).At(sample_index) - buffer.At(1).At(sample_index)) };

				//Is the difference higher than the threshold?
				if (difference > THRESHOLD)
				{
					return false;
				}

				total_sum += buffer.At(0).At(sample_index);
				total_sum += buffer.At(1).At(sample_index);
			}

			//If we got here, we didn't detect a difference higher than the threshold - If we were fed all 0's, we should just return the previous value (if one was supplied).
			if (total_sum == 0.0f && previous_value)
			{
				return *previous_value;
			}

			return true;
		}

	}

}