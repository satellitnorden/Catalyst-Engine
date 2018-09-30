#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Matrix4.h>
#include <Math/Vector4.h>

class VulkanDynamicUniformData final
{

public:

	//The maximum number of points lights.
	static constexpr uint8 MAXIMUM_NUMBER_OF_POINT_LIGHTS{ 8 };

	//The maximum number of spot lights.
	static constexpr uint8 MAXIMUM_NUMBER_OF_SPOT_LIGHTS{ 8 };

	//Camera data.
	Vector4 _CameraFieldOfViewCosine;
	Matrix4 _InverseCameraMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _OriginViewMatrix;
	Matrix4 _ViewMatrix;
	Vector4 _CameraForwardVector;
	Vector4 _CameraWorldPosition;

	//Directional light data.
	Vector4 _DirectionalLightIntensity;
	Matrix4 _DirectionalLightViewMatrix;
	Vector4 _DirectionalLightDirection;
	Vector4 _DirectionalLightColor;
	Vector4 _DirectionalLightScreenSpacePosition;

	//Environment data.
	float _EnvironmentBlend;

	//General data.
	float _DeltaTime;
	float _TotalGameTime;

	//Point light data.
	int32 _NumberOfPointLights;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_POINT_LIGHTS> _PointLightAttenuationDistances;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_POINT_LIGHTS> _PointLightIntensities;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_POINT_LIGHTS> _PointLightColors;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_POINT_LIGHTS> _PointLightWorldPositions;

	//Spot light data.
	int32 _NumberOfSpotLights;
	uint32 _Padding2{ 0 };
	uint64 _Padding3{ 0 };
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightAttenuationDistances;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightIntensities;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightInnerCutoffAngles;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightOuterCutoffAngles;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightColors;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightDirections;
	StaticArray<Vector4, MAXIMUM_NUMBER_OF_SPOT_LIGHTS> _SpotLightWorldPositions;

	//Physics data.
	Vector4 _WindSpeed;
	Vector4 _WindDirection;

};

static_assert(sizeof(VulkanDynamicUniformData) == 1904, "X");

static_assert(offsetof(VulkanDynamicUniformData, _CameraFieldOfViewCosine) == 0, "X");
static_assert(offsetof(VulkanDynamicUniformData, _InverseCameraMatrix) == 16, "X");
static_assert(offsetof(VulkanDynamicUniformData, _InverseProjectionMatrix) == 80, "X");
static_assert(offsetof(VulkanDynamicUniformData, _OriginViewMatrix) == 144, "X");
static_assert(offsetof(VulkanDynamicUniformData, _ViewMatrix) == 208, "X");
static_assert(offsetof(VulkanDynamicUniformData, _CameraForwardVector) == 272, "X");
static_assert(offsetof(VulkanDynamicUniformData, _CameraWorldPosition) == 288, "X");

static_assert(offsetof(VulkanDynamicUniformData, _DirectionalLightIntensity) == 304, "X");
static_assert(offsetof(VulkanDynamicUniformData, _DirectionalLightViewMatrix) == 320, "X");
static_assert(offsetof(VulkanDynamicUniformData, _DirectionalLightDirection) == 384, "X");
static_assert(offsetof(VulkanDynamicUniformData, _DirectionalLightColor) == 400, "X");
static_assert(offsetof(VulkanDynamicUniformData, _DirectionalLightScreenSpacePosition) == 416, "X");

static_assert(offsetof(VulkanDynamicUniformData, _EnvironmentBlend) == 432, "X");

static_assert(offsetof(VulkanDynamicUniformData, _DeltaTime) == 436, "X");
static_assert(offsetof(VulkanDynamicUniformData, _TotalGameTime) == 440, "X");

static_assert(offsetof(VulkanDynamicUniformData, _NumberOfPointLights) == 444, "X");
static_assert(offsetof(VulkanDynamicUniformData, _PointLightAttenuationDistances) == 448, "X");
static_assert(offsetof(VulkanDynamicUniformData, _PointLightIntensities) == 576, "X");
static_assert(offsetof(VulkanDynamicUniformData, _PointLightColors) == 704, "X");
static_assert(offsetof(VulkanDynamicUniformData, _PointLightWorldPositions) == 832, "X");

static_assert(offsetof(VulkanDynamicUniformData, _NumberOfSpotLights) == 960, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightAttenuationDistances) == 976, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightIntensities) == 1104, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightInnerCutoffAngles) == 1232, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightOuterCutoffAngles) == 1360, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightColors) == 1488, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightDirections) == 1616, "X");
static_assert(offsetof(VulkanDynamicUniformData, _SpotLightWorldPositions) == 1744, "X");

static_assert(offsetof(VulkanDynamicUniformData, _WindSpeed) == 1872, "X");
static_assert(offsetof(VulkanDynamicUniformData, _WindDirection) == 1888, "X");