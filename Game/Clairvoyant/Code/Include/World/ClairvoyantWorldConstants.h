#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Clairvoyant world constants
namespace ClairvoyantWorldConstants
{
	constexpr float SAND_BLEND_BEGIN{ 0.0f };
	constexpr float SAND_BLEND_END{ 1'000.0f };

	constexpr float SNOW_BLEND_BEGIN{ 7'500.0f };
	constexpr float SNOW_BLEND_END{ 8'500.0f };
}

//Enumeration covering all Clairvoyant biomes.
enum class ClairvoyantBiome : uint8
{
	KingsMeadow,
	NorthHollow,

	NumberOfClairvoyantBiomes
};

//Enumeration covering all Clairvoyant terrain materials.
enum class ClairvoyantTerrainMaterial : uint8
{
	Grass_1,
	Rock_1,
	Snow_1,

	NumberOfClairvoyantTerrainMaterials
};