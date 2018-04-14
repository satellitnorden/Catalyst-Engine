#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class ParticleMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 mipmapLevels;

	//The width.
	uint32 width;

	//The height.
	uint32 height;

	//The albedo data.
	DynamicArray<DynamicArray<byte>> albedoData;

};