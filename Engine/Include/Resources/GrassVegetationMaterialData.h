#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class GrassVegetationMaterialData final
{

public:

	//The number of mipmap levels for the mask texture.
	uint8 _MaskMipmapLevels;

	//The mask width.
	uint32 _MaskWidth;

	//The mask height.
	uint32 _MaskHeight;

	//The mask data.
	DynamicArray<DynamicArray<byte>> _MaskData;

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

};