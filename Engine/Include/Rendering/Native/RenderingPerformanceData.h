#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class RenderingPerformanceData final
{

public:

	//The names.
	DynamicArray<const char *RESTRICT> _Names;

	//The query pool.
	QueryPoolHandle _QueryPool;

};