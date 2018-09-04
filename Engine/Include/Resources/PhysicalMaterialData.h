#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class PhysicalMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The albedo data.
	DynamicArray<DynamicArray<byte>> _AlbedoData;

	//The normal map data.
	DynamicArray<DynamicArray<byte>> _NormalMapData;

	//The material properties data.
	DynamicArray<DynamicArray<byte>> _MaterialPropertiesData;

};