#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class EnvironmentMaterialData final
{

public:

	//The resolution of the diffuse.
	uint32 _DiffuseResolution;

	//The diffuse data.
	DynamicArray<float> _DiffuseData;

	//The resolution of the diffuse irradiance.
	uint32 _DiffuseIrradianceResolution;

	//The diffuse irradiance data.
	DynamicArray<float> _DiffuseIrradianceData;

};