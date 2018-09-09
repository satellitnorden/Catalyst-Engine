#if defined(CATALYST_ENABLE_OCEAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class OceanMaterialData final
{

public:

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The normal map data.
	DynamicArray<DynamicArray<byte>> _NormalMapData;

};
#endif