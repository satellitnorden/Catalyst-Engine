#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Matrix4.h>
#include <Math/Vector4.h>

class DynamicUniformData final
{

public:

	Matrix4 _DirectionalLightViewMatrix;
	Matrix4 _InversePerceiverMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _ViewMatrix;

	Vector4<float> _DirectionalLightColor;
	Vector4<float> _DirectionalLightDirection;
	Vector4<float> _PerceiverWorldPosition;
	Vector4<float> _WindDirection;

	float _DeltaTime;
	float _DirectionalLightIntensity;
	float _EnvironmentBlend;
	float _TotalTime;
	float _WindSpeed;
};

static_assert(sizeof(DynamicUniformData) == 340, "Size of dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DirectionalLightViewMatrix) == 0, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InversePerceiverMatrix) == 64, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InverseProjectionMatrix) == 128, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ViewMatrix) == 192, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DirectionalLightColor) == 256, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightDirection) == 272, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverWorldPosition) == 288, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _WindDirection) == 304, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DeltaTime) == 320, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightIntensity) == 324, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _EnvironmentBlend) == 328, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _TotalTime) == 332, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _WindSpeed) == 336, "Offset inside dynamic uniform data is wrong!");