#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ParticleMaterialData final
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

};