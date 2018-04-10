#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class VegetationMaterialData final
{

public:

	//The width.
	uint32 width;

	//The height.
	uint32 height;

	//The albedo data.
	DynamicArray<DynamicArray<byte>> albedoData;

};