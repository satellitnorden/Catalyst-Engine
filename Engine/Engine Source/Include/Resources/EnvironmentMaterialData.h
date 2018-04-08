#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class EnvironmentMaterialData final
{

public:

	//The resolution of the diffuse.
	uint32 diffuseResolution;

	//The diffuse data.
	DynamicArray<float> diffuseData;

	//The resolution of the diffuse irradiance.
	uint32 diffuseIrradianceResolution;

	//The diffuse irradiance data.
	DynamicArray<float> diffuseIrradianceData;

};