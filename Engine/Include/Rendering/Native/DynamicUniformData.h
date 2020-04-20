#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

	Matrix4x4 _ViewMatrixMinusOne;
	Matrix4x4 _UNUSED1;
	Matrix4x4 _InversePerceiverMatrix;
	Matrix4x4 _InverseProjectionMatrix;
	Matrix4x4 _PerceiverMatrix;
	Matrix4x4 _ProjectionMatrix;
	Matrix4x4 _ViewMatrix;

	Vector4<float> _UNUSEDX;
	Vector4<float> _UNUSEDY;
	Vector4<float> _UNUSED6;
	Vector4<float> _PerceiverForwardVector;
	Vector4<float> _PerceiverWorldPosition;

	Vector2<float> _ScaledResolution;
	Vector2<float> _InverseScaledResolution;
	Vector2<float> _PreviousFrameJitter;
	Vector2<float> _CurrentFrameJitter;

	float _DeltaTime;
	float _UNUSEDZ;
	float _GlobalRandomSeed1;
	float _GlobalRandomSeed2;
	float _GlobalRandomSeed3;
	float _UNUSED5;
	float _TotalTime;
	float _WindSpeed;

	int32 _AmbientOcclusionMode;
	int32 _MotionBlurMode;
	int32 _UNUSED3;
	int32 _ShadowsMode;
	int32 _UNUSED4;

	float _UNUSED2;
	float _BloomIntensity;
	float _UNUSEDWAT;
	float _UNUSEDW;

	float32 _AspectRatio;

	uint32 _CurrentBlueNoiseTextureIndex;
	float _CurrentBlueNoiseTextureOffsetX;
	float _CurrentBlueNoiseTextureOffsetY;
	float _ViewDistance;
	float _CloudDensity;
	float _Wetness;
	float _NearPlane;
	float _FarPlane;

	uint32 _TerrainHeightMapTextureIndex;
	uint32 _TerrainIndexMapTextureIndex;
	uint32 _TerrainBlendMapTextureIndex;
	float32 _TerrainMapResolution;

};

static_assert(sizeof(DynamicUniformData) == 680, "Size of dynamic uniform data is wrong!");