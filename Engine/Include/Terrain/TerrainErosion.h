#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

namespace TerrainErosion
{

	/*
	*	Performs an erosion simulation on the given height map.
	*/
	FORCE_INLINE static void SimulateErosion(const uint32 resolution, const uint64 iterations, float *const RESTRICT height_map) NOEXCEPT
	{
		//Define constants.
		constexpr float SEDIMENT_DEPOSIT_FRACTION{ 0.0001f };
		constexpr float SEDIMENT_WITHDRAW_FRACTION{ 0.0001f };
		constexpr float EVAPORATION_SPEED{ 0.01f };

		for (uint64 i{ 0 }; i < iterations; ++i)
		{
			//Keep track of the droplet's sediment.
			float sediment{ 0.0f };
			float humidity{ 1.0f };

			//Randomize the droplet's starting coordinate.
			Vector2<uint32> current{ CatalystRandomMath::RandomIntegerInRange<uint32>(0, resolution - 1), CatalystRandomMath::RandomIntegerInRange<uint32>(0, resolution - 1) };

			for (;;)
			{
				//Retrieve the current height.
				const float current_height{ height_map[current._X + (current._Y * resolution)] };

				//Decide which direction to go.
				const StaticArray<Vector2<uint32>, 8> coordinates
				{
					Vector2<uint32>(current._X > 0 ? current._X - 1 : 0, current._Y > 0 ? current._Y - 1 : 0),
					Vector2<uint32>(current._X > 0 ? current._X - 1 : 0, current._Y),
					Vector2<uint32>(current._X > 0 ? current._X - 1 : 0, current._Y < (resolution - 1) ? current._Y + 1 : current._Y),

					Vector2<uint32>(current._X, current._Y > 0 ? current._Y - 1 : 0),
					Vector2<uint32>(current._X, current._Y < (resolution - 1) ? current._Y + 1 : current._Y),

					Vector2<uint32>(current._X < (resolution - 1) ? current._X + 1 : current._X, current._Y > 0 ? current._Y - 1 : 0),
					Vector2<uint32>(current._X < (resolution - 1) ? current._X + 1 : current._X, current._Y),
					Vector2<uint32>(current._X < (resolution - 1) ? current._X + 1 : current._X, current._Y < (resolution - 1) ? current._Y + 1 : current._Y)
				};
				
				float largest_height_difference{ -FLOAT_MAXIMUM };
				Vector2<uint32> next_coordinate{ 0, 0 };

				for (uint8 j{ 0 }; j < 8; ++j)
				{
					const float sample_height{ height_map[coordinates[j]._X + (coordinates[j]._Y * resolution)] };
					
					if (sample_height < current_height)
					{
						const float height_difference{ current_height - sample_height };

						if (largest_height_difference < height_difference)
						{
							largest_height_difference = height_difference;
							next_coordinate = coordinates[j];
						}
					}
				}

				//If a neighbor pixel couldn't be found with a height smaller than the current, break out of the loop.
				if (largest_height_difference < 0.0f)
				{
					//Deposit the remaining sediment to the current pixel.
					height_map[current._X + (current._Y * resolution)] += sediment;

					break;
				}

				const float sediment_to_deposit{ sediment * SEDIMENT_DEPOSIT_FRACTION * (1.0f - humidity) };

				sediment -= sediment_to_deposit;
				height_map[current._X + (current._Y * resolution)] += sediment_to_deposit;

				const float sediment_to_withdraw{ largest_height_difference * SEDIMENT_WITHDRAW_FRACTION * humidity };

				sediment += sediment_to_withdraw;
				height_map[current._X + (current._Y * resolution)] -= sediment_to_withdraw;

				//Update the humidity.
				humidity -= EVAPORATION_SPEED;

				if (humidity <= 0.0f)
				{
					//Deposit the remaining sediment to the current pixel.
					height_map[current._X + (current._Y * resolution)] += sediment;

					break;
				}

				//Update the coordinate.
				current = next_coordinate;
			}
		}
	}

}