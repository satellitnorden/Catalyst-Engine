#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

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

static_assert(sizeof(DynamicUniformData) == 544, "Size of dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _PerceiverMatrixMinusOne) == 0, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ProjectionMatrixMinusOne) == 64, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InversePerceiverMatrix) == 128, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InverseProjectionMatrix) == 192, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverMatrix) == 256, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ProjectionMatrix) == 320, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ViewMatrix) == 384, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DirectionalLightColor) == 448, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightDirection) == 464, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverForwardVector) == 480, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverWorldPosition) == 496, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DeltaTime) == 512, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightIntensity) == 516, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _EnvironmentBlend) == 520, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed1) == 524, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed2) == 528, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed3) == 532, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _TotalTime) == 536, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _WindSpeed) == 540, "Offset inside dynamic uniform data is wrong!");