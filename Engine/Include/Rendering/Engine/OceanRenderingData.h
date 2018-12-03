#if defined(CATALYST_ENABLE_OCEAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
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

	data._Color = Vector3<float>(0.0f, 1.0f, 1.0f);
	data._DeformationWeightDistanceSquared = 250.0f * 250.0f;
	data._ColorWeightDistanceSquared = 250.0f * 250.0f;
	data._FoamWeightDistanceSquared = 5.0f * 5.0f;
	data._NormalSmoothingFactor = 0.9f;
	data._TextureScaling = 0.1f;
	
	return data;
}
#endif