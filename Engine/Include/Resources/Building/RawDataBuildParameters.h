#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class RawDataBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _Output{ nullptr };

	//The resource resource identifier.
	const char *RESTRICT _ResourceIdentifier{ nullptr };

	//The data.
	DynamicArray<byte> *RESTRICT _Data;

};