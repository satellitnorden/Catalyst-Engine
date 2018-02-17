#pragma once

//Engine core.
#include <EngineCore.h>

class TerrainMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 mipmapLevels;

	//The width of the first layer.
	uint32 firstLayerWidth;

	//The height of the first layer.
	uint32 firstLayerHeight;

	//The first layer albedo data.
	DynamicArray<DynamicArray<byte>> firstLayerAlbedoData;

	//The first layer normal map data.
	DynamicArray<DynamicArray<byte>> firstLayerNormalMapData;

	//The first layer properties data.
	DynamicArray<DynamicArray<byte>> firstLayerPropertiesData;

};