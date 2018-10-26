//Header file.
#include <Terrain/TerrainSchedule.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

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

	constexpr StaticArray<TerrainBorder, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> BORDERS
	{
		TerrainBorder::None,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,

		TerrainBorder::Upper,
		TerrainBorder::Upper | TerrainBorder::Right,
		TerrainBorder::Right,
		TerrainBorder::Right | TerrainBorder::Lower,
		TerrainBorder::Lower,
		TerrainBorder::Lower | TerrainBorder::Left,
		TerrainBorder::Left,
		TerrainBorder::Upper | TerrainBorder::Left,
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
void TerrainSchedule::GenerateNewSchedule(const GridPoint2 previous, const GridPoint2 current) NOEXCEPT
{
	//Calculate the delta.
	const GridPoint2 delta{ current - previous };

	//Sort the schedule in different direction depending on the delta.
	if (delta._X < 0)
	{
		//Generate the starting schedule.
		GenerateStartingSchedule(GridPoint2(current._X, previous._Y));

		SortingAlgorithms::InsertionSort<TerrainScheduleItem>(_Items.Begin(), _Items.End(), nullptr, [](const void *const RESTRICT userData, const TerrainScheduleItem *const RESTRICT first, const TerrainScheduleItem *const RESTRICT second)
		{
			return first->_GridPoint._X > second->_GridPoint._X;
		});
	}

	else if (delta._X > 0)
	{
		//Generate the starting schedule.
		GenerateStartingSchedule(GridPoint2(current._X, previous._Y));

		SortingAlgorithms::InsertionSort<TerrainScheduleItem>(_Items.Begin(), _Items.End(), nullptr, [](const void *const RESTRICT userData, const TerrainScheduleItem *const RESTRICT first, const TerrainScheduleItem *const RESTRICT second)
		{
			return first->_GridPoint._X < second->_GridPoint._X;
		});
	}

	else if (delta._Y < 0)
	{
		//Generate the starting schedule.
		GenerateStartingSchedule(GridPoint2(previous._X, current._Y));

		SortingAlgorithms::InsertionSort<TerrainScheduleItem>(_Items.Begin(), _Items.End(), nullptr, [](const void *const RESTRICT userData, const TerrainScheduleItem *const RESTRICT first, const TerrainScheduleItem *const RESTRICT second)
		{
			return first->_GridPoint._Y > second->_GridPoint._Y;
		});
	}

	else if (delta._Y > 0)
	{
		//Generate the starting schedule.
		GenerateStartingSchedule(GridPoint2(previous._X, current._Y));

		SortingAlgorithms::InsertionSort<TerrainScheduleItem>(_Items.Begin(), _Items.End(), nullptr, [](const void *const RESTRICT userData, const TerrainScheduleItem *const RESTRICT first, const TerrainScheduleItem *const RESTRICT second)
		{
			return first->_GridPoint._Y < second->_GridPoint._Y;
		});
	}

	else
	{
		GenerateStartingSchedule(current);
	}

	//Reset the current item.
	_CurrentItem = 0;
}

/*
*	Generates the starting schedule.
*/
void TerrainSchedule::GenerateStartingSchedule(const GridPoint2 current) NOEXCEPT
{
	#define SET_ITEM(SCHEDULE_INDEX, PATCH_INDEX)																		\
	{																													\
		_Items[SCHEDULE_INDEX]._Index = PATCH_INDEX;																	\
		_Items[SCHEDULE_INDEX]._GridPoint = current + TerrainScheduleConstants::GRID_POINT_OFFSETS[PATCH_INDEX];		\
		_Items[SCHEDULE_INDEX]._Borders = TerrainScheduleConstants::BORDERS[PATCH_INDEX];								\
		_Items[SCHEDULE_INDEX]._PatchSizeMultiplier = TerrainScheduleConstants::PATCH_SIZE_MULTIPLIERS[PATCH_INDEX];	\
	}

	//For now, just generate the starting schedule.
	SET_ITEM(0, TerrainScheduleConstants::CENTER_INDEX);

	SET_ITEM(1, TerrainScheduleConstants::UPPER_LAYER_ZERO_INDEX);
	SET_ITEM(2, TerrainScheduleConstants::UPPER_RIGHT_LAYER_ZERO_INDEX);
	SET_ITEM(3, TerrainScheduleConstants::RIGHT_LAYER_ZERO_INDEX);
	SET_ITEM(4, TerrainScheduleConstants::LOWER_RIGHT_LAYER_ZERO_INDEX);
	SET_ITEM(5, TerrainScheduleConstants::LOWER_LAYER_ZERO_INDEX);
	SET_ITEM(6, TerrainScheduleConstants::LOWER_LEFT_LAYER_ZERO_INDEX);
	SET_ITEM(7, TerrainScheduleConstants::LEFT_LAYER_ZERO_INDEX);
	SET_ITEM(8, TerrainScheduleConstants::UPPER_LEFT_LAYER_ZERO_INDEX);

	SET_ITEM(9, TerrainScheduleConstants::UPPER_LAYER_ONE_INDEX);
	SET_ITEM(10, TerrainScheduleConstants::UPPER_RIGHT_LAYER_ONE_INDEX);
	SET_ITEM(11, TerrainScheduleConstants::RIGHT_LAYER_ONE_INDEX);
	SET_ITEM(12, TerrainScheduleConstants::LOWER_RIGHT_LAYER_ONE_INDEX);
	SET_ITEM(13, TerrainScheduleConstants::LOWER_LAYER_ONE_INDEX);
	SET_ITEM(14, TerrainScheduleConstants::LOWER_LEFT_LAYER_ONE_INDEX);
	SET_ITEM(15, TerrainScheduleConstants::LEFT_LAYER_ONE_INDEX);
	SET_ITEM(16, TerrainScheduleConstants::UPPER_LEFT_LAYER_ONE_INDEX);

	SET_ITEM(17, TerrainScheduleConstants::UPPER_LAYER_TWO_INDEX);
	SET_ITEM(18, TerrainScheduleConstants::UPPER_RIGHT_LAYER_TWO_INDEX);
	SET_ITEM(19, TerrainScheduleConstants::RIGHT_LAYER_TWO_INDEX);
	SET_ITEM(20, TerrainScheduleConstants::LOWER_RIGHT_LAYER_TWO_INDEX);
	SET_ITEM(21, TerrainScheduleConstants::LOWER_LAYER_TWO_INDEX);
	SET_ITEM(22, TerrainScheduleConstants::LOWER_LEFT_LAYER_TWO_INDEX);
	SET_ITEM(23, TerrainScheduleConstants::LEFT_LAYER_TWO_INDEX);
	SET_ITEM(24, TerrainScheduleConstants::UPPER_LEFT_LAYER_TWO_INDEX);

	SET_ITEM(25, TerrainScheduleConstants::UPPER_LAYER_THREE_INDEX);
	SET_ITEM(26, TerrainScheduleConstants::UPPER_RIGHT_LAYER_THREE_INDEX);
	SET_ITEM(27, TerrainScheduleConstants::RIGHT_LAYER_THREE_INDEX);
	SET_ITEM(28, TerrainScheduleConstants::LOWER_RIGHT_LAYER_THREE_INDEX);
	SET_ITEM(29, TerrainScheduleConstants::LOWER_LAYER_THREE_INDEX);
	SET_ITEM(30, TerrainScheduleConstants::LOWER_LEFT_LAYER_THREE_INDEX);
	SET_ITEM(31, TerrainScheduleConstants::LEFT_LAYER_THREE_INDEX);
	SET_ITEM(32, TerrainScheduleConstants::UPPER_LEFT_LAYER_THREE_INDEX);

	SET_ITEM(33, TerrainScheduleConstants::UPPER_LAYER_FOUR_INDEX);
	SET_ITEM(34, TerrainScheduleConstants::UPPER_RIGHT_LAYER_FOUR_INDEX);
	SET_ITEM(35, TerrainScheduleConstants::RIGHT_LAYER_FOUR_INDEX);
	SET_ITEM(36, TerrainScheduleConstants::LOWER_RIGHT_LAYER_FOUR_INDEX);
	SET_ITEM(37, TerrainScheduleConstants::LOWER_LAYER_FOUR_INDEX);
	SET_ITEM(38, TerrainScheduleConstants::LOWER_LEFT_LAYER_FOUR_INDEX);
	SET_ITEM(39, TerrainScheduleConstants::LEFT_LAYER_FOUR_INDEX);
	SET_ITEM(40, TerrainScheduleConstants::UPPER_LEFT_LAYER_FOUR_INDEX);

	SET_ITEM(41, TerrainScheduleConstants::UPPER_LAYER_FIVE_INDEX);
	SET_ITEM(42, TerrainScheduleConstants::UPPER_RIGHT_LAYER_FIVE_INDEX);
	SET_ITEM(43, TerrainScheduleConstants::RIGHT_LAYER_FIVE_INDEX);
	SET_ITEM(44, TerrainScheduleConstants::LOWER_RIGHT_LAYER_FIVE_INDEX);
	SET_ITEM(45, TerrainScheduleConstants::LOWER_LAYER_FIVE_INDEX);
	SET_ITEM(46, TerrainScheduleConstants::LOWER_LEFT_LAYER_FIVE_INDEX);
	SET_ITEM(47, TerrainScheduleConstants::LEFT_LAYER_FIVE_INDEX);
	SET_ITEM(48, TerrainScheduleConstants::UPPER_LEFT_LAYER_FIVE_INDEX);

	SET_ITEM(49, TerrainScheduleConstants::UPPER_LAYER_SIX_INDEX);
	SET_ITEM(50, TerrainScheduleConstants::UPPER_RIGHT_LAYER_SIX_INDEX);
	SET_ITEM(51, TerrainScheduleConstants::RIGHT_LAYER_SIX_INDEX);
	SET_ITEM(52, TerrainScheduleConstants::LOWER_RIGHT_LAYER_SIX_INDEX);
	SET_ITEM(53, TerrainScheduleConstants::LOWER_LAYER_SIX_INDEX);
	SET_ITEM(54, TerrainScheduleConstants::LOWER_LEFT_LAYER_SIX_INDEX);
	SET_ITEM(55, TerrainScheduleConstants::LEFT_LAYER_SIX_INDEX);
	SET_ITEM(56, TerrainScheduleConstants::UPPER_LEFT_LAYER_SIX_INDEX);

	SET_ITEM(57, TerrainScheduleConstants::UPPER_LAYER_SEVEN_INDEX);
	SET_ITEM(58, TerrainScheduleConstants::UPPER_RIGHT_LAYER_SEVEN_INDEX);
	SET_ITEM(59, TerrainScheduleConstants::RIGHT_LAYER_SEVEN_INDEX);
	SET_ITEM(60, TerrainScheduleConstants::LOWER_RIGHT_LAYER_SEVEN_INDEX);
	SET_ITEM(61, TerrainScheduleConstants::LOWER_LAYER_SEVEN_INDEX);
	SET_ITEM(62, TerrainScheduleConstants::LOWER_LEFT_LAYER_SEVEN_INDEX);
	SET_ITEM(63, TerrainScheduleConstants::LEFT_LAYER_SEVEN_INDEX);
	SET_ITEM(64, TerrainScheduleConstants::UPPER_LEFT_LAYER_SEVEN_INDEX);
}