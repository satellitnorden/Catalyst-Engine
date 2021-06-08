#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class SurfaceDescription final
{

public:
	
	//The albedo.
	Vector3<float32> _Albedo;

	//The normal.
	Vector3<float32> _Normal;

	//The roughness.
	float32 _Roughness;

	//The metallic.
	float32 _Metallic;

	//The ambient occlusion.
	float32 _AmbientOcclusion;

	//The emissive.
	float32 _Emissive;

};