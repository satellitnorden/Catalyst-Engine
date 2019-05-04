#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

class DynamicUniformData final
{

public:

	Matrix4 _InversePerceiverMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _PerceiverMatrix;
	Matrix4 _PreviousViewMatrix;
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

static_assert(sizeof(DynamicUniformData) == 480, "Size of dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _InversePerceiverMatrix) == 0, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InverseProjectionMatrix) == 64, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverMatrix) == 128, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PreviousViewMatrix) == 192, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ProjectionMatrix) == 256, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ViewMatrix) == 320, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DirectionalLightColor) == 384, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightDirection) == 400, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverForwardVector) == 416, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverWorldPosition) == 432, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DeltaTime) == 448, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightIntensity) == 452, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _EnvironmentBlend) == 456, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed1) == 460, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed2) == 464, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed3) == 468, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _TotalTime) == 472, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _WindSpeed) == 476, "Offset inside dynamic uniform data is wrong!");