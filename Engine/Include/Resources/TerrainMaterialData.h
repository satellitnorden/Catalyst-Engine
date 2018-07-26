#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
	DynamicArray<DynamicArray<byte>> firstLayerMaterialPropertiesData;

	//The width of the second layer.
	uint32 secondLayerWidth;

	//The height of the second layer.
	uint32 secondLayerHeight;

	//The second layer albedo data.
	DynamicArray<DynamicArray<byte>> secondLayerAlbedoData;

	//The second layer normal map data.
	DynamicArray<DynamicArray<byte>> secondLayerNormalMapData;

	//The second layer properties data.
	DynamicArray<DynamicArray<byte>> secondLayerMaterialPropertiesData;

	//The width of the third layer.
	uint32 thirdLayerWidth;

	//The height of the third layer.
	uint32 thirdLayerHeight;

	//The third layer albedo data.
	DynamicArray<DynamicArray<byte>> thirdLayerAlbedoData;

	//The third layer normal map data.
	DynamicArray<DynamicArray<byte>> thirdLayerNormalMapData;

	//The third layer properties data.
	DynamicArray<DynamicArray<byte>> thirdLayerMaterialPropertiesData;

	//The width of the fourth layer.
	uint32 fourthLayerWidth;

	//The height of the fourth layer.
	uint32 fourthLayerHeight;

	//The fourth layer albedo data.
	DynamicArray<DynamicArray<byte>> fourthLayerAlbedoData;

	//The fourth layer normal map data.
	DynamicArray<DynamicArray<byte>> fourthLayerNormalMapData;

	//The fourth layer properties data.
	DynamicArray<DynamicArray<byte>> fourthLayerMaterialPropertiesData;

	//The width of the fifth layer.
	uint32 fifthLayerWidth;

	//The height of the fifth layer.
	uint32 fifthLayerHeight;

	//The fifth layer albedo data.
	DynamicArray<DynamicArray<byte>> fifthLayerAlbedoData;

	//The fifth layer normal map data.
	DynamicArray<DynamicArray<byte>> fifthLayerNormalMapData;

	//The fifth layer properties data.
	DynamicArray<DynamicArray<byte>> fifthLayerMaterialPropertiesData;

};