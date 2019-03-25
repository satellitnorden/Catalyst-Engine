#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class OceanMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The normal data.
	DynamicArray<DynamicArray<byte>> _NormalData;

	//The foam data.
	DynamicArray<DynamicArray<byte>> _FoamData;

};