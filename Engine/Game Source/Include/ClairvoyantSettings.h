#pragma once

/*
*	The project name.
*/
#define PROJECT_NAME			"Clairvoyant"

/*
*	The game system class.
*/
#define GAME_SYSTEM_CLASS		ClairvoyantGameSystem

/*
*	The game materials folder.
*/
#define GAME_MATERIALS_FOLDER	"../../../Game/Clairvoyant/Assets/Final/Materials/"

/*
*	The game models folder.
*/
#define GAME_MODELS_FOLDER		"../../Game Content/Clairvoyant/Models/"

/*
*	The game textures folder.
*/
#define GAME_RAW_TEXTURES_FOLDER	"../../../Game/Clairvoyant/Assets/Raw/Textures/"

/*
*	Enumeration covering all game daily quests.
*/
enum class GameDailyQuests : uint8
{
	WorldArchitectGenerateHeightMap,
	NumberOfGameDailyQuests
};