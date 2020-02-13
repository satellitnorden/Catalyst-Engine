#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

	Matrix4 _ViewMatrixMinusOne;
	Matrix4 _UNUSED1;
	Matrix4 _InversePerceiverMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _PerceiverMatrix;
	Matrix4 _ProjectionMatrix;
	Matrix4 _ViewMatrix;

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
	float _ChromaticAberrationIntensity;
	float _VolumetricLightingIntensity;

	float _DepthOfFieldFocusDistance;

	int32 _ActiveBlueNoiseTextureIndex;
	float _ActiveBlueNoiseTextureOffsetX;
	float _ActiveBlueNoiseTextureOffsetY;
	float _ViewDistance;
	float _CloudDensity;
	float _Wetness;
	float _NearPlane;
	float _FarPlane;

};

static_assert(sizeof(DynamicUniformData) == 664, "Size of dynamic uniform data is wrong!");