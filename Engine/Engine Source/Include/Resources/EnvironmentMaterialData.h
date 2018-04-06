#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class EnvironmentMaterialData final
{

public:

	//The resolution of the environment material.
	uint32 resolution;

	//The diffuse data.
	DynamicArray<float> diffuseData;

	//The diffuse irradiance data.
	DynamicArray<float> diffuseIrradianceData;

};