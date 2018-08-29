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
	static constexpr uint8 MaximumNumberOfPointLights{ 8 };

	//The maximum number of spot lights.
	static constexpr uint8 MaximumNumberOfSpotLights{ 8 };

	//Camera data.
	Vector4 cameraFieldOfViewCosine;
	Matrix4 inverseCameraMatrix;
	Matrix4 inverseProjectionMatrix;
	Matrix4 originViewMatrix;
	Matrix4 viewMatrix;
	Vector4 cameraForwardVector;
	Vector4 cameraWorldPosition;

	//Directional light data.
	Vector4 directionalLightIntensity;
	Matrix4 directionalLightViewMatrix;
	Vector4 directionalLightDirection;
	Vector4 directionalLightColor;
	Vector4 directionalLightScreenSpacePosition;

	//Environment data.
	float environmentBlend;

	//General data.
	float deltaTime;
	float totalGameTime;

	//Point light data.
	int32 numberOfPointLights;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightAttenuationDistances;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightIntensities;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightColors;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightWorldPositions;

	//Spot light data.
	int32 numberOfSpotLights;
	uint32 padding2{ 0 };
	uint64 padding3{ 0 };
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightAttenuationDistances;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightIntensities;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightInnerCutoffAngles;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightOuterCutoffAngles;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightColors;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightDirections;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightWorldPositions;

	//Physics data.
	Vector4 windStrength;
	Vector4 windDirection;

};

static_assert(sizeof(VulkanDynamicUniformData) == 1904, "X");

static_assert(offsetof(VulkanDynamicUniformData, cameraFieldOfViewCosine) == 0, "X");
static_assert(offsetof(VulkanDynamicUniformData, inverseCameraMatrix) == 16, "X");
static_assert(offsetof(VulkanDynamicUniformData, inverseProjectionMatrix) == 80, "X");
static_assert(offsetof(VulkanDynamicUniformData, originViewMatrix) == 144, "X");
static_assert(offsetof(VulkanDynamicUniformData, viewMatrix) == 208, "X");
static_assert(offsetof(VulkanDynamicUniformData, cameraForwardVector) == 272, "X");
static_assert(offsetof(VulkanDynamicUniformData, cameraWorldPosition) == 288, "X");

static_assert(offsetof(VulkanDynamicUniformData, directionalLightIntensity) == 304, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightViewMatrix) == 320, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightDirection) == 384, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightColor) == 400, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightScreenSpacePosition) == 416, "X");

static_assert(offsetof(VulkanDynamicUniformData, environmentBlend) == 432, "X");

static_assert(offsetof(VulkanDynamicUniformData, deltaTime) == 436, "X");
static_assert(offsetof(VulkanDynamicUniformData, totalGameTime) == 440, "X");

static_assert(offsetof(VulkanDynamicUniformData, numberOfPointLights) == 444, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightAttenuationDistances) == 448, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightIntensities) == 576, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightColors) == 704, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightWorldPositions) == 832, "X");

static_assert(offsetof(VulkanDynamicUniformData, numberOfSpotLights) == 960, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightAttenuationDistances) == 976, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightIntensities) == 1104, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightInnerCutoffAngles) == 1232, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightOuterCutoffAngles) == 1360, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightColors) == 1488, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightDirections) == 1616, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightWorldPositions) == 1744, "X");

static_assert(offsetof(VulkanDynamicUniformData, windStrength) == 1872, "X");
static_assert(offsetof(VulkanDynamicUniformData, windDirection) == 1888, "X");