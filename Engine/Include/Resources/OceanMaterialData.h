#pragma once

//Core.
#include <Core/General/EngineCore.h>

class OceanMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 mipmapLevels;

	//The width.
	uint32 width;

	//The height.
	uint32 height;

	//The normal map data.
	DynamicArray<DynamicArray<byte>> normalMapData;

};