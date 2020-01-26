#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

class ResourceCollectionBuildParameters final
{

public:

	//The output file.
	const char *RESTRICT _Output;

	//The folder in which to search for resource files.
	const char* RESTRICT _Folder;

};