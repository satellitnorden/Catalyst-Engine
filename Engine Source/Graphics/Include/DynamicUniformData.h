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

	//Point light data.
	uint64 numberOfPointLights;
	uint64 padding1;
	Vector4 pointLightAttenuationDistances[MaximumNumberOfPointLights];
	Vector4 pointLightIntensities[MaximumNumberOfPointLights];
	Vector4 pointLightColors[MaximumNumberOfPointLights];
	Vector4 pointLightWorldPositions[MaximumNumberOfPointLights];

	//Spot light data.
	uint64 numberOfSpotLights;
	uint64 padding2;
	Vector4 spotLightAttenuationDistances[MaximumNumberOfSpotLights];
	Vector4 spotLightIntensities[MaximumNumberOfSpotLights];
	Vector4 spotLightInnerCutoffAngles[MaximumNumberOfSpotLights];
	Vector4 spotLightOuterCutoffAngles[MaximumNumberOfSpotLights];
	Vector4 spotLightColors[MaximumNumberOfSpotLights];
	Vector4 spotLightDirections[MaximumNumberOfSpotLights];
	Vector4 spotLightWorldPositions[MaximumNumberOfSpotLights];

};