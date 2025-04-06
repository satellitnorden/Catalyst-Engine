#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ComponentStatistics final
{

public:

	//The CPU memory usage, in bytes.
	uint64 _CPUMemoryUsage;

	//The GPU memory usage, in bytes.
	uint64 _GPUMemoryUsage;

};