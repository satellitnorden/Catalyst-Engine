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
	Matrix4 _UNUSED;
	Matrix4 _InversePerceiverMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _PerceiverMatrix;
	Matrix4 _ProjectionMatrix;
	Matrix4 _ViewMatrix;

	Vector4<float> _DirectionalLightColor;
	Vector4<float> _PerceiverWorldPositionMinusOne;
	Vector4<float> _PerceiverForwardVector;
	Vector4<float> _PerceiverVelocity;
	Vector4<float> _PerceiverWorldPosition;

	Vector2<float> _ScaledResolution;
	Vector2<float> _InverseScaledResolution;
	Vector2<float> _PreviousFrameJitter;
	Vector2<float> _CurrentFrameJitter;

	float _DeltaTime;
	float _DirectionalLightIntensity;
	float _GlobalRandomSeed1;
	float _GlobalRandomSeed2;
	float _GlobalRandomSeed3;
	float _PerceiverRotationVelocity;
	float _TotalTime;
	float _WindSpeed;

	int32 _AntiAliasingMode;
	int32 _DiffuseIrradianceMode;
	int32 _VolumetricLightingMode;

};

static_assert(sizeof(DynamicUniformData) == 604, "Size of dynamic uniform data is wrong!");