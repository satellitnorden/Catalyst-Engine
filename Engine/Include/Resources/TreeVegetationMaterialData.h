#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TreeVegetationMaterialData final
{

public:

	//The number of mipmap levels for the trunk.
	uint8 _TrunkMipmapLevels;

	//The trunk width.
	uint32 _TrunkWidth;

	//The trunk height.
	uint32 _TrunkHeight;

	//The trunk albedo data.
	DynamicArray<DynamicArray<byte>> _TrunkAlbedoData;

	//The trunk normal map data.
	DynamicArray<DynamicArray<byte>> _TrunkNormalMapData;

	//The trunk material properties data.
	DynamicArray<DynamicArray<byte>> _TrunkMaterialPropertiesData;

};