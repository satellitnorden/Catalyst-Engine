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

	float _DeltaTime;
	float _DirectionalLightIntensity;
	float _GlobalRandomSeed1;
	float _GlobalRandomSeed2;
	float _GlobalRandomSeed3;
	float _PerceiverRotationVelocity;
	float _TotalTime;
	float _WindSpeed;
};

static_assert(sizeof(DynamicUniformData) == 560, "Size of dynamic uniform data is wrong!");