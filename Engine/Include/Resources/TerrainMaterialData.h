#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TerrainMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The width of the first layer.
	uint32 _FirstLayerWidth;

	//The height of the first layer.
	uint32 _FirstLayerHeight;

	//The first layer albedo data.
	DynamicArray<DynamicArray<byte>> _FirstLayerAlbedoData;

	//The first layer normal map data.
	DynamicArray<DynamicArray<byte>> _FirstLayerNormalMapData;

	//The first layer properties data.
	DynamicArray<DynamicArray<byte>> _FirstLayerMaterialPropertiesData;

	//The width of the second layer.
	uint32 _SecondLayerWidth;

	//The height of the second layer.
	uint32 _SecondLayerHeight;

	//The second layer albedo data.
	DynamicArray<DynamicArray<byte>> _SecondLayerAlbedoData;

	//The second layer normal map data.
	DynamicArray<DynamicArray<byte>> _SecondLayerNormalMapData;

	//The second layer properties data.
	DynamicArray<DynamicArray<byte>> _SecondLayerMaterialPropertiesData;

	//The width of the third layer.
	uint32 _ThirdLayerWidth;

	//The height of the third layer.
	uint32 _ThirdLayerHeight;

	//The third layer albedo data.
	DynamicArray<DynamicArray<byte>> _ThirdLayerAlbedoData;

	//The third layer normal map data.
	DynamicArray<DynamicArray<byte>> _ThirdLayerNormalMapData;

	//The third layer properties data.
	DynamicArray<DynamicArray<byte>> _ThirdLayerMaterialPropertiesData;

	//The width of the fourth layer.
	uint32 _FourthLayerWidth;

	//The height of the fourth layer.
	uint32 _FourthLayerHeight;

	//The fourth layer albedo data.
	DynamicArray<DynamicArray<byte>> _FourthLayerAlbedoData;

	//The fourth layer normal map data.
	DynamicArray<DynamicArray<byte>> _FourthLayerNormalMapData;

	//The fourth layer properties data.
	DynamicArray<DynamicArray<byte>> _FourthLayerMaterialPropertiesData;

	//The width of the fifth layer.
	uint32 _FifthLayerWidth;

	//The height of the fifth layer.
	uint32 _FifthLayerHeight;

	//The fifth layer albedo data.
	DynamicArray<DynamicArray<byte>> _FifthLayerAlbedoData;

	//The fifth layer normal map data.
	DynamicArray<DynamicArray<byte>> _FifthLayerNormalMapData;

	//The fifth layer properties data.
	DynamicArray<DynamicArray<byte>> _FifthLayerMaterialPropertiesData;

};