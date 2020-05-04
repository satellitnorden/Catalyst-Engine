#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

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

	Vector2<float32> _InverseScaledResolution;
	Vector2<float32> _PreviousFrameJitter;
	Vector2<float32> _CurrentFrameJitter;

	float32 _DeltaTime;
	float32 _GlobalRandomSeed1;
	float32 _GlobalRandomSeed2;
	float32 _GlobalRandomSeed3;
	float32 _TotalTime;
	float32 _WindSpeed;

	float _BloomIntensity;

	float32 _AspectRatio;

	uint32 _CurrentBlueNoiseTextureIndex;
	float _CurrentBlueNoiseTextureOffsetX;
	float _CurrentBlueNoiseTextureOffsetY;

	float _ViewDistance;
	float _MaximumSkyTextureMipmapLevel;
	float _Wetness;
	float _NearPlane;
	float _FarPlane;

	uint32 _TerrainHeightMapTextureIndex;
	uint32 _TerrainIndexMapTextureIndex;
	uint32 _TerrainBlendMapTextureIndex;
	float32 _TerrainMapResolution;

	float32 _SkyIntensity;

};

static_assert(sizeof(DynamicUniformData) == 460, "Size of dynamic uniform data is wrong!");