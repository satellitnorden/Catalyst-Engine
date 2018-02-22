#pragma once

/*
*	The project name.
*/
#define PROJECT_NAME				"Clairvoyant"

/*
*	The game materials folder.
*/
#define GAME_MATERIALS_FOLDER		"../../../Game Resources/Final/Materials/"

/*
*	The game textures folder.
*/
#define GAME_RAW_TEXTURES_FOLDER	"../../../Game Resources/Raw/Textures/"

/*
*	Enumeration covering all game daily quests.
*/
enum class GameDailyQuests : uint8
{
	WorldArchitectGenerateHeightMap,
	NumberOfGameDailyQuests
};