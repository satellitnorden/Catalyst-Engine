#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class TextureCubeData final
{

public:

	//The resolution.
	uint32 _Resolution;

	//The number of mipmap levels.
	uint8 _MipmapLevels;

	//The data.
	DynamicArray<DynamicArray<float32>> _Data;

};