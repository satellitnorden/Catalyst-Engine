#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

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
	Vector4 directionalLightDirection;
	Vector4 directionalLightColor;
	Vector4 directionalLightScreenSpacePosition;

	//General data.
	float totalGameTime;

	//Point light data.
	int32 numberOfPointLights;
	uint64 padding1{ 0 };
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

};

static_assert(sizeof(VulkanDynamicUniformData) == 1808, "X");

static_assert(offsetof(VulkanDynamicUniformData, cameraFieldOfViewCosine) == 0, "X");
static_assert(offsetof(VulkanDynamicUniformData, inverseCameraMatrix) == 16, "X");
static_assert(offsetof(VulkanDynamicUniformData, inverseProjectionMatrix) == 80, "X");
static_assert(offsetof(VulkanDynamicUniformData, originViewMatrix) == 144, "X");
static_assert(offsetof(VulkanDynamicUniformData, viewMatrix) == 208, "X");
static_assert(offsetof(VulkanDynamicUniformData, cameraForwardVector) == 272, "X");
static_assert(offsetof(VulkanDynamicUniformData, cameraWorldPosition) == 288, "X");

static_assert(offsetof(VulkanDynamicUniformData, directionalLightIntensity) == 304, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightDirection) == 320, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightColor) == 336, "X");
static_assert(offsetof(VulkanDynamicUniformData, directionalLightScreenSpacePosition) == 352, "X");

static_assert(offsetof(VulkanDynamicUniformData, totalGameTime) == 368, "X");

static_assert(offsetof(VulkanDynamicUniformData, numberOfPointLights) == 372, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightAttenuationDistances) == 384, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightIntensities) == 512, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightColors) == 640, "X");
static_assert(offsetof(VulkanDynamicUniformData, pointLightWorldPositions) == 768, "X");

static_assert(offsetof(VulkanDynamicUniformData, numberOfSpotLights) == 896, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightAttenuationDistances) == 912, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightIntensities) == 1040, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightInnerCutoffAngles) == 1168, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightOuterCutoffAngles) == 1296, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightColors) == 1424, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightDirections) == 1552, "X");
static_assert(offsetof(VulkanDynamicUniformData, spotLightWorldPositions) == 1680, "X");