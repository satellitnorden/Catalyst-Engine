#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Clairvoyant world constants
namespace ClairvoyantWorldConstants
{
	constexpr float GRASS_CUTOFF_DISTANCE{ 250.0f };
	constexpr float BRANCHES_CUTOFF_DISTANCE{ 500.0f };
	constexpr float PLANTS_CUTOFF_DISTANCE{ 1'000.0f };
	constexpr float ROCKS_CUTOFF_DISTANCE{ 2'000.0f };
}

//Enumeration covering all Clairvoyant biomes.
enum class ClairvoyantBiome : uint8
{
	NorthHollow,
	SilverGarden,
	WalkerValley,

	NumberOfClairvoyantBiomes
};

//Enumeration covering all Clairvoyant terrain materials.
enum class ClairvoyantTerrainMaterial : uint8
{
	Cliff_1,
	Cliff_2,
	Grass_1,
	Grass_2,
	Leaves_1,
	Roots_1,
	Sand_1,
	Snow_1,

	NumberOfClairvoyantTerrainMaterials
};