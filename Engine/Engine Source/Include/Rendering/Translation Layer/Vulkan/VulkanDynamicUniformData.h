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
	float directionalLightIntensity;
	uint64 padding1{ 0 };
	Vector4 directionalLightDirection;
	Vector4 directionalLightColor;
	Vector4 directionalLightScreenSpacePosition;

	//General data.
	float totalGameTime;

	//Point light data.
	uint64 numberOfPointLights;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightAttenuationDistances;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightIntensities;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightColors;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightWorldPositions;

	//Spot light data.
	uint64 numberOfSpotLights;
	uint64 padding2{ 0 };
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightAttenuationDistances;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightIntensities;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightInnerCutoffAngles;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightOuterCutoffAngles;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightColors;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightDirections;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightWorldPositions;

};