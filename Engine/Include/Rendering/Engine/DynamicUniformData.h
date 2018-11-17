#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Matrix4.h>
#include <Math/Vector4.h>

class DynamicUniformData final
{

public:

	//The maximum number of points lights.
	static constexpr uint8 MAXIMUM_NUMBER_OF_POINT_LIGHTS{ 8 };

	//The maximum number of spot lights.
	static constexpr uint8 MAXIMUM_NUMBER_OF_SPOT_LIGHTS{ 8 };

	//Viewer data.
	Vector4 _ViewerFieldOfViewCosine;
	Matrix4 _InverseViewerMatrix;
	Matrix4 _InverseProjectionMatrix;
	Matrix4 _OriginViewMatrix;
	Matrix4 _ViewMatrix;
	Vector4 _ViewerForwardVector;
	Vector4 _ViewerWorldPosition;

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

static_assert(sizeof(DynamicUniformData) == 1904, "X");

static_assert(offsetof(DynamicUniformData, _ViewerFieldOfViewCosine) == 0, "X");
static_assert(offsetof(DynamicUniformData, _InverseViewerMatrix) == 16, "X");
static_assert(offsetof(DynamicUniformData, _InverseProjectionMatrix) == 80, "X");
static_assert(offsetof(DynamicUniformData, _OriginViewMatrix) == 144, "X");
static_assert(offsetof(DynamicUniformData, _ViewMatrix) == 208, "X");
static_assert(offsetof(DynamicUniformData, _ViewerForwardVector) == 272, "X");
static_assert(offsetof(DynamicUniformData, _ViewerWorldPosition) == 288, "X");

static_assert(offsetof(DynamicUniformData, _DirectionalLightIntensity) == 304, "X");
static_assert(offsetof(DynamicUniformData, _DirectionalLightViewMatrix) == 320, "X");
static_assert(offsetof(DynamicUniformData, _DirectionalLightDirection) == 384, "X");
static_assert(offsetof(DynamicUniformData, _DirectionalLightColor) == 400, "X");
static_assert(offsetof(DynamicUniformData, _DirectionalLightScreenSpacePosition) == 416, "X");

static_assert(offsetof(DynamicUniformData, _EnvironmentBlend) == 432, "X");

static_assert(offsetof(DynamicUniformData, _DeltaTime) == 436, "X");
static_assert(offsetof(DynamicUniformData, _TotalGameTime) == 440, "X");

static_assert(offsetof(DynamicUniformData, _NumberOfPointLights) == 444, "X");
static_assert(offsetof(DynamicUniformData, _PointLightAttenuationDistances) == 448, "X");
static_assert(offsetof(DynamicUniformData, _PointLightIntensities) == 576, "X");
static_assert(offsetof(DynamicUniformData, _PointLightColors) == 704, "X");
static_assert(offsetof(DynamicUniformData, _PointLightWorldPositions) == 832, "X");

static_assert(offsetof(DynamicUniformData, _NumberOfSpotLights) == 960, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightAttenuationDistances) == 976, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightIntensities) == 1104, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightInnerCutoffAngles) == 1232, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightOuterCutoffAngles) == 1360, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightColors) == 1488, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightDirections) == 1616, "X");
static_assert(offsetof(DynamicUniformData, _SpotLightWorldPositions) == 1744, "X");

static_assert(offsetof(DynamicUniformData, _WindSpeed) == 1872, "X");
static_assert(offsetof(DynamicUniformData, _WindDirection) == 1888, "X");