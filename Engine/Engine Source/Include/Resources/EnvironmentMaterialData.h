#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class EnvironmentMaterialData final
{

public:

	//The albedo data.
	DynamicArray<float> albedoData;

	//The diffuse data.
	DynamicArray<float> diffuseData;

	//The diffuse irradiance data.
	DynamicArray<float> diffuseIrradianceData;

};