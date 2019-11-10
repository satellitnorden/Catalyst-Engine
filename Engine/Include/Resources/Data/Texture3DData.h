#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class Texture3DData final
{

public:

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	//The depth.
	uint32 _Depth;

	//The data.
	DynamicArray<DynamicArray<byte>> _Data;

};