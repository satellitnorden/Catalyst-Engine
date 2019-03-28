#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class TextureCubeData final
{

public:

	//The resolution.
	uint32 _Resolution;

	//The data.
	DynamicArray<float> _Data;

};