#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TerrainMaterialData final
{

public:

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The albedo data.
	DynamicArray<byte> _AlbedoData;

	//The normal map data.
	DynamicArray<byte> _NormalMapData;

	//The material properties data.
	DynamicArray<byte> _MaterialPropertiesData;

};