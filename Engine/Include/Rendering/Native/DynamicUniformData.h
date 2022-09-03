#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Padding.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

	Matrix4x4 _InverseCameraToClipMatrix;
	Matrix4x4 _InverseWorldToCameraMatrix;
	Matrix4x4 _PreviousWorldToClipMatrix;
	Matrix4x4 _UserInterfaceMatrix;
	Matrix4x4 _WorldToClipMatrix;
	Matrix4x4 _WorldToCameraMatrix;

	Vector4<float32> _SkyLightRadiance;

	Vector4<float32> _SkyLightDirection;

	Vector4<float32> _CameraForwardVector;
	Vector4<float32> _CameraWorldPosition;

	Vector4<float32> _UpperSkyColor;
	Vector4<float32> _LowerSkyColor;

	Vector4<int32> _CurrentWorldGridCell;

	Vector2<float32> _ScaledResolution;
	Vector2<float32> _InverseScaledResolution;
	Vector2<float32> _CurrentFrameJitter;
	Vector2<float32> _PreviousFramejitter;

	Vector2<float32> _HeightMapCoordinateOffset;

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

	float32 _CameraAbsoluteHeight;

	uint32 _SkyMode;
	float32 _SkyIntensity;
	float32 _StarIntensity;

	float32 _VolumetricLightingDistance;
	float32 _VolumetricLightingHeight;
	float32 _VolumetricLightingThickness;

	float32 _WorldGridSize;

};

static_assert(sizeof(DynamicUniformData) == 632, "Size of dynamic uniform data is wrong!");