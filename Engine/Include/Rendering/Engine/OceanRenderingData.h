#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/Vector3.h>

class OceanRenderingData final
{

public:

	//The color.
	Vector3<float> _Color;

	//Padding.
	Padding<4> _Padding;

	//Denotes the distance at which the deformation reaches full weight.
	float _DeformationWeightDistanceSquared;

	//Denotes the distance at which the color reaches full weight.
	float _ColorWeightDistanceSquared;

	//Denotes the distance at which the foam reaches full weight.
	float _FoamWeightDistanceSquared;

	//The normal smoothing factor.
	float _NormalSmoothingFactor;

	//The texture scaling.
	float _TextureScaling;

};

/*
*	Returns the default ocean rendering data.
*/
static OceanRenderingData GetDefaultOceanRenderingData() NOEXCEPT
{
	OceanRenderingData data;

	data._Color = Vector3<float>(0.0f, 0.375f, 0.5f);
	data._DeformationWeightDistanceSquared = 500.0f * 500.0f;
	data._ColorWeightDistanceSquared = 500.0f * 500.0f;
	data._FoamWeightDistanceSquared = 2.5f * 2.5f;
	data._NormalSmoothingFactor = 0.75f;
	data._TextureScaling = 0.1f;
	
	return data;
}