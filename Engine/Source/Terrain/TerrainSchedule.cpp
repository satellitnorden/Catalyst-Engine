//Header file.
#include <Terrain/TerrainSchedule.h>

//Terrain schedule constants.
namespace TerrainScheduleConstants
{
	constexpr uint8 CENTER_INDEX{ 0 };

	constexpr uint8 UPPER_LAYER_ZERO_INDEX{ 1 };
	constexpr uint8 UPPER_RIGHT_LAYER_ZERO_INDEX{ 2 };
	constexpr uint8 RIGHT_LAYER_ZERO_INDEX{ 3 };
	constexpr uint8 LOWER_RIGHT_LAYER_ZERO_INDEX{ 4 };
	constexpr uint8 LOWER_LAYER_ZERO_INDEX{ 5 };
	constexpr uint8 LOWER_LEFT_LAYER_ZERO_INDEX{ 6 };
	constexpr uint8 LEFT_LAYER_ZERO_INDEX{ 7 };
	constexpr uint8 UPPER_LEFT_LAYER_ZERO_INDEX{ 8 };

	constexpr uint8 UPPER_LAYER_ONE_INDEX{ 9 };
	constexpr uint8 UPPER_RIGHT_LAYER_ONE_INDEX{ 10 };
	constexpr uint8 RIGHT_LAYER_ONE_INDEX{ 11 };
	constexpr uint8 LOWER_RIGHT_LAYER_ONE_INDEX{ 12 };
	constexpr uint8 LOWER_LAYER_ONE_INDEX{ 13 };
	constexpr uint8 LOWER_LEFT_LAYER_ONE_INDEX{ 14 };
	constexpr uint8 LEFT_LAYER_ONE_INDEX{ 15 };
	constexpr uint8 UPPER_LEFT_LAYER_ONE_INDEX{ 16 };

	constexpr uint8 UPPER_LAYER_TWO_INDEX{ 17 };
	constexpr uint8 UPPER_RIGHT_LAYER_TWO_INDEX{ 18 };
	constexpr uint8 RIGHT_LAYER_TWO_INDEX{ 19 };
	constexpr uint8 LOWER_RIGHT_LAYER_TWO_INDEX{ 20 };
	constexpr uint8 LOWER_LAYER_TWO_INDEX{ 21 };
	constexpr uint8 LOWER_LEFT_LAYER_TWO_INDEX{ 22 };
	constexpr uint8 LEFT_LAYER_TWO_INDEX{ 23 };
	constexpr uint8 UPPER_LEFT_LAYER_TWO_INDEX{ 24 };

	constexpr uint8 UPPER_LAYER_THREE_INDEX{ 25 };
	constexpr uint8 UPPER_RIGHT_LAYER_THREE_INDEX{ 26 };
	constexpr uint8 RIGHT_LAYER_THREE_INDEX{ 27 };
	constexpr uint8 LOWER_RIGHT_LAYER_THREE_INDEX{ 28 };
	constexpr uint8 LOWER_LAYER_THREE_INDEX{ 29 };
	constexpr uint8 LOWER_LEFT_LAYER_THREE_INDEX{ 30 };
	constexpr uint8 LEFT_LAYER_THREE_INDEX{ 31 };
	constexpr uint8 UPPER_LEFT_LAYER_THREE_INDEX{ 32 };

	constexpr uint8 UPPER_LAYER_FOUR_INDEX{ 33 };
	constexpr uint8 UPPER_RIGHT_LAYER_FOUR_INDEX{ 34 };
	constexpr uint8 RIGHT_LAYER_FOUR_INDEX{ 35 };
	constexpr uint8 LOWER_RIGHT_LAYER_FOUR_INDEX{ 36 };
	constexpr uint8 LOWER_LAYER_FOUR_INDEX{ 37 };
	constexpr uint8 LOWER_LEFT_LAYER_FOUR_INDEX{ 38 };
	constexpr uint8 LEFT_LAYER_FOUR_INDEX{ 39 };
	constexpr uint8 UPPER_LEFT_LAYER_FOUR_INDEX{ 40 };

	constexpr uint8 UPPER_LAYER_FIVE_INDEX{ 41 };
	constexpr uint8 UPPER_RIGHT_LAYER_FIVE_INDEX{ 42 };
	constexpr uint8 RIGHT_LAYER_FIVE_INDEX{ 43 };
	constexpr uint8 LOWER_RIGHT_LAYER_FIVE_INDEX{ 44 };
	constexpr uint8 LOWER_LAYER_FIVE_INDEX{ 45 };
	constexpr uint8 LOWER_LEFT_LAYER_FIVE_INDEX{ 46 };
	constexpr uint8 LEFT_LAYER_FIVE_INDEX{ 47 };
	constexpr uint8 UPPER_LEFT_LAYER_FIVE_INDEX{ 48 };

	constexpr uint8 UPPER_LAYER_SIX_INDEX{ 49 };
	constexpr uint8 UPPER_RIGHT_LAYER_SIX_INDEX{ 50 };
	constexpr uint8 RIGHT_LAYER_SIX_INDEX{ 51 };
	constexpr uint8 LOWER_RIGHT_LAYER_SIX_INDEX{ 52 };
	constexpr uint8 LOWER_LAYER_SIX_INDEX{ 53 };
	constexpr uint8 LOWER_LEFT_LAYER_SIX_INDEX{ 54 };
	constexpr uint8 LEFT_LAYER_SIX_INDEX{ 55 };
	constexpr uint8 UPPER_LEFT_LAYER_SIX_INDEX{ 56 };

	constexpr uint8 UPPER_LAYER_SEVEN_INDEX{ 57 };
	constexpr uint8 UPPER_RIGHT_LAYER_SEVEN_INDEX{ 58 };
	constexpr uint8 RIGHT_LAYER_SEVEN_INDEX{ 59 };
	constexpr uint8 LOWER_RIGHT_LAYER_SEVEN_INDEX{ 60 };
	constexpr uint8 LOWER_LAYER_SEVEN_INDEX{ 61 };
	constexpr uint8 LOWER_LEFT_LAYER_SEVEN_INDEX{ 62 };
	constexpr uint8 LEFT_LAYER_SEVEN_INDEX{ 63 };
	constexpr uint8 UPPER_LEFT_LAYER_SEVEN_INDEX{ 64 };

	constexpr StaticArray<GridPoint2, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> GRID_POINT_OFFSETS
	{
		GridPoint2(0, 0),

		GridPoint2(0, -1),
		GridPoint2(1, -1),
		GridPoint2(1, 0),
		GridPoint2(1, 1),
		GridPoint2(0, 1),
		GridPoint2(-1, 1),
		GridPoint2(-1, 0),
		GridPoint2(-1, -1),

		GridPoint2(0, -1) * 3,
		GridPoint2(1, -1) * 3,
		GridPoint2(1, 0) * 3,
		GridPoint2(1, 1) * 3,
		GridPoint2(0, 1) * 3,
		GridPoint2(-1, 1) * 3,
		GridPoint2(-1, 0) * 3,
		GridPoint2(-1, -1) * 3,

		GridPoint2(0, -1) * 9,
		GridPoint2(1, -1) * 9,
		GridPoint2(1, 0) * 9,
		GridPoint2(1, 1) * 9,
		GridPoint2(0, 1) * 9,
		GridPoint2(-1, 1) * 9,
		GridPoint2(-1, 0) * 9,
		GridPoint2(-1, -1) * 9,

		GridPoint2(0, -1) * 27,
		GridPoint2(1, -1) * 27,
		GridPoint2(1, 0) * 27,
		GridPoint2(1, 1) * 27,
		GridPoint2(0, 1) * 27,
		GridPoint2(-1, 1) * 27,
		GridPoint2(-1, 0) * 27,
		GridPoint2(-1, -1) * 27,

		GridPoint2(0, -1) * 81,
		GridPoint2(1, -1) * 81,
		GridPoint2(1, 0) * 81,
		GridPoint2(1, 1) * 81,
		GridPoint2(0, 1) * 81,
		GridPoint2(-1, 1) * 81,
		GridPoint2(-1, 0) * 81,
		GridPoint2(-1, -1) * 81,

		GridPoint2(0, -1) * 243,
		GridPoint2(1, -1) * 243,
		GridPoint2(1, 0) * 243,
		GridPoint2(1, 1) * 243,
		GridPoint2(0, 1) * 243,
		GridPoint2(-1, 1) * 243,
		GridPoint2(-1, 0) * 243,
		GridPoint2(-1, -1) * 243,

		GridPoint2(0, -1) * 729,
		GridPoint2(1, -1) * 729,
		GridPoint2(1, 0) * 729,
		GridPoint2(1, 1) * 729,
		GridPoint2(0, 1) * 729,
		GridPoint2(-1, 1) * 729,
		GridPoint2(-1, 0) * 729,
		GridPoint2(-1, -1) * 729,

		GridPoint2(0, -1) * 2'187,
		GridPoint2(1, -1) * 2'187,
		GridPoint2(1, 0) * 2'187,
		GridPoint2(1, 1) * 2'187,
		GridPoint2(0, 1) * 2'187,
		GridPoint2(-1, 1) * 2'187,
		GridPoint2(-1, 0) * 2'187,
		GridPoint2(-1, -1) * 2'187,
	};

	constexpr StaticArray<float, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> PATCH_SIZE_MULTIPLIERS
	{
		1.0f,

		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,

		3.0f,
		3.0f,
		3.0f,
		3.0f,
		3.0f,
		3.0f,
		3.0f,
		3.0f,

		9.0f,
		9.0f,
		9.0f,
		9.0f,
		9.0f,
		9.0f,
		9.0f,
		9.0f,

		27.0f,
		27.0f,
		27.0f,
		27.0f,
		27.0f,
		27.0f,
		27.0f,
		27.0f,

		81.0f,
		81.0f,
		81.0f,
		81.0f,
		81.0f,
		81.0f,
		81.0f,
		81.0f,

		243.0f,
		243.0f,
		243.0f,
		243.0f,
		243.0f,
		243.0f,
		243.0f,
		243.0f,

		729.0f,
		729.0f,
		729.0f,
		729.0f,
		729.0f,
		729.0f,
		729.0f,
		729.0f,

		2'187.0f,
		2'187.0f,
		2'187.0f,
		2'187.0f,
		2'187.0f,
		2'187.0f,
		2'187.0f,
		2'187.0f,
	};
}

/*
*	Generates a new schedule based on the delta.
*/
void TerrainSchedule::GenerateNewSchedule(const GridPoint2 delta) NOEXCEPT
{
	#define SET_ITEM(index)																				\
	{																									\
		_Items[index]._Index = index;																	\
		_Items[index]._GridPoint = TerrainScheduleConstants::GRID_POINT_OFFSETS[index];					\
		_Items[index]._PatchSizeMultiplier = TerrainScheduleConstants::PATCH_SIZE_MULTIPLIERS[index];	\
	}
	
	//For now, just generate the starting schedule.
	SET_ITEM(TerrainScheduleConstants::CENTER_INDEX);
}