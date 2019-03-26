#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class ResourceCollectionBuildParameters final
{

public:

	//The output file.
	const char *RESTRICT _Output;

	//File paths to all resources to put in the resource collection.
	DynamicArray<const char *RESTRICT> _Resources;

};