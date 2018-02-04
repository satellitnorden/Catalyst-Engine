#pragma once

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

//Engine core.
#include <EngineCore.h>

//Math.
#include <Matrix4.h>
#include <Vector4.h>

class DynamicUniformData final
{

public:

	//Camera data.
	Matrix4 inverseCameraMatrix;
	Matrix4 inverseProjectionMatrix;
	Matrix4 originViewMatrix;
	Matrix4 viewMatrix;
	Vector4 cameraWorldPosition;

	//Directional light data.
	float directionalLightIntensity;
	uint64 padding1{ 0 };
	Vector4 directionalLightDirection;
	Vector4 directionalLightColor;
	Vector4 directionalLightScreenSpacePosition;

	//Point light data.
	uint64 numberOfPointLights;
	uint64 padding2{ 0 };
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightAttenuationDistances;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightIntensities;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightColors;
	StaticArray<Vector4, MaximumNumberOfPointLights> pointLightWorldPositions;

	//Spot light data.
	uint64 numberOfSpotLights;
	uint64 padding3{ 0 };
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightAttenuationDistances;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightIntensities;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightInnerCutoffAngles;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightOuterCutoffAngles;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightColors;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightDirections;
	StaticArray<Vector4, MaximumNumberOfSpotLights> spotLightWorldPositions;

};