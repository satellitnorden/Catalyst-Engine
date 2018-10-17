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
	Vector3 _SunColor;

	//The sun intensity.
	float _SunIntensity;

	//The wind speed.
	float _WindSpeed;

	//The volumetric fog distance.
	float _VolumetricFogDistance;

};