#pragma once

//Core.
#include <Core/EngineCore.h>

class VegetationMaterialData final
{

public:

	//The number of mask mipmap levels.
	uint8 maskMipmapLevels;

	//The number of remaining mipmap levels
	uint8 remainingMipmapLevels;

	//The width.
	uint32 width;

	//The height.
	uint32 height;

	//The mask data.
	DynamicArray<DynamicArray<byte>> maskData;

	//The albedo data.
	DynamicArray<DynamicArray<byte>> albedoData;

	//The normal map data.
	DynamicArray<DynamicArray<byte>> normalMapData;

	//The properties data.
	DynamicArray<DynamicArray<byte>> propertiesData;

};