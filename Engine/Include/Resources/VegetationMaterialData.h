#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class VegetationMaterialData final
{

public:

	//The number of mask mipmap levels.
	uint8 _MaskMipmapLevels;

	//The number of remaining mipmap levels
	uint8 _RemainingMipmapLevels;

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The mask data.
	DynamicArray<DynamicArray<byte>> _MaskData;

	//The albedo data.
	DynamicArray<DynamicArray<byte>> _AlbedoData;

	//The normal map data.
	DynamicArray<DynamicArray<byte>> _NormalMapData;

	//The properties data.
	DynamicArray<DynamicArray<byte>> _PropertiesData;

};