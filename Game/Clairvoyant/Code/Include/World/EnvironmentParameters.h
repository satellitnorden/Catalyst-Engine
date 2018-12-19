#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector3.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>

class EnvironmentParameters final
{

public:

	//The environment material.
	EnvironmentMaterial _EnvironmentMaterial;

	//The sun color.
	Vector3<float> _SunColor;

	//The sun intensity.
	float _SunIntensity;

	//The wind speed.
	float _WindSpeed;

	//The volumetric fog density.
	float _VolumetricFogDensity;

	//The depth of field distance.
	float _DepthOfFieldDistance;

};