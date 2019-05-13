#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

	Matrix4 _PerceiverMatrixMinusSeven;
	Matrix4 _ProjectionMatrixMinusSeven;
	Matrix4 _PerceiverMatrixMinusSix;
	Matrix4 _ProjectionMatrixMinusSix;
	Matrix4 _PerceiverMatrixMinusFive;
	Matrix4 _ProjectionMatrixMinusFive;
	Matrix4 _PerceiverMatrixMinusFour;
	Matrix4 _ProjectionMatrixMinusFour;
	Matrix4 _PerceiverMatrixMinusThree;
	Matrix4 _ProjectionMatrixMinusThree;
	Matrix4 _PerceiverMatrixMinusTwo;
	Matrix4 _ProjectionMatrixMinusTwo;
	Matrix4 _PerceiverMatrixMinusOne;
	Matrix4 _ProjectionMatrixMinusOne;
	Matrix4 _InversePerceiverMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _PerceiverMatrix;
	Matrix4 _ProjectionMatrix;
	Matrix4 _ViewMatrix;

	Vector4<float> _DirectionalLightColor;
	Vector4<float> _DirectionalLightDirection;
	Vector4<float> _PerceiverForwardVector;
	Vector4<float> _PerceiverWorldPosition;

	float _DeltaTime;
	float _DirectionalLightIntensity;
	float _EnvironmentBlend;
	float _GlobalRandomSeed1;
	float _GlobalRandomSeed2;
	float _GlobalRandomSeed3;
	float _TotalTime;
	float _WindSpeed;
};

static_assert(sizeof(DynamicUniformData) == 1312, "Size of dynamic uniform data is wrong!");