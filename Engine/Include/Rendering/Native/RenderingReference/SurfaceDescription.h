#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class SurfaceDescription final
{

public:
	
	//The albedo.
	Vector3<float> _Albedo;

	//The normal.
	Vector3<float> _Normal;

	//The roughness.
	float _Roughness;

	//The metallic.
	float _Metallic;

	//The ambient occlusion.
	float _AmbientOcclusion;

};