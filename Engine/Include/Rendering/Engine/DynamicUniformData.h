#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix4.h>
#include <Math/General/Vector4.h>

class DynamicUniformData final
{

public:

	Matrix4 _DirectionalLightViewMatrix;
	Matrix4 _InversePerceiverMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _PerceiverMatrix;
	Matrix4 _ProjectionMatrix;
	Matrix4 _ViewMatrix;

	Vector4<float> _DirectionalLightColor;
	Vector4<float> _DirectionalLightDirection;
	Vector4<float> _PerceiverWorldPosition;
	Vector4<float> _WindDirection;

	float _DeltaTime;
	float _DirectionalLightIntensity;
	float _EnvironmentBlend;
	float _GlobalRandomSeed;
	float _TotalTime;
	float _WindSpeed;
};

static_assert(sizeof(DynamicUniformData) == 472, "Size of dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DirectionalLightViewMatrix) == 0, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InversePerceiverMatrix) == 64, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _InverseProjectionMatrix) == 128, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverMatrix) == 192, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ProjectionMatrix) == 256, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _ViewMatrix) == 320, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DirectionalLightColor) == 384, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightDirection) == 400, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _PerceiverWorldPosition) == 416, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _WindDirection) == 432, "Offset inside dynamic uniform data is wrong!");

static_assert(offsetof(DynamicUniformData, _DeltaTime) == 448, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _DirectionalLightIntensity) == 452, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _EnvironmentBlend) == 456, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _GlobalRandomSeed) == 460, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _TotalTime) == 464, "Offset inside dynamic uniform data is wrong!");
static_assert(offsetof(DynamicUniformData, _WindSpeed) == 468, "Offset inside dynamic uniform data is wrong!");