#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

	Matrix4x4 _InversePerceiverToClipMatrix;
	Matrix4x4 _InverseWorldToPerceiverMatrix;
	Matrix4x4 _PreviousWorldToClipMatrix;
	Matrix4x4 _WorldToClipMatrix;
	Matrix4x4 _WorldToPerceiverMatrix;

	Vector4<float32> _PerceiverForwardVector;
	Vector4<float32> _PerceiverWorldPosition;

	Vector2<float32> _ScaledResolution;
	Vector2<float32> _InverseScaledResolution;
	Vector2<float32> _CurrentFrameJitter;

	float32 _DeltaTime;
	float32 _GlobalRandomSeed1;
	float32 _GlobalRandomSeed2;
	float32 _GlobalRandomSeed3;
	float32 _TotalTime;
	float32 _WindSpeed;

	float32 _BloomIntensity;

	float32 _AspectRatio;

	uint32 _CurrentBlueNoiseTextureIndex;
	float32 _CurrentBlueNoiseTextureOffsetX;
	float32 _CurrentBlueNoiseTextureOffsetY;

	float32 _ViewDistance;
	float32 _MaximumSkyTextureMipmapLevel;
	float32 _Wetness;
	float32 _NearPlane;
	float32 _FarPlane;

	uint32 _TerrainHeightMapTextureIndex;
	uint32 _TerrainIndexMapTextureIndex;
	uint32 _TerrainBlendMapTextureIndex;
	float32 _TerrainMapResolution;

	float32 _SkyIntensity;

	float32 _VolumetricLightingDistance;
	float32 _VolumetricLightingHeight;
	float32 _VolumetricLightingThickness;

	Vector2<float32> _PreviousFramejitter;

	Vector3<int32> _CurrentWorldGridCell;
	Padding<4> _Padding1;
	float32 _WorldGridSize;

};

static_assert(sizeof(DynamicUniformData) == 500, "Size of dynamic uniform data is wrong!");