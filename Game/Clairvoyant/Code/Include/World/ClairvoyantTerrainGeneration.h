#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class TerrainDisplacementInformation;
class TerrainMaterial;
class TerrainProperties;
class Vector3;
class Vector4;

namespace ClairvoyantTerrainGeneration
{

	/*
	*	Generates the height.
	*/
	void GenerateHeight(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height) NOEXCEPT;

	/*
	*	Generates the layer weights.
	*/
	void GenerateLayerWeights(const TerrainProperties &properties, const Vector3 &worldPosition, const Vector3 &normal, Vector4 *const RESTRICT layerWeights) NOEXCEPT;

	/*
	*	Generates the patch properties.
	*/
	void GeneratePatchProperties(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT, TerrainDisplacementInformation *const RESTRICT displacementInformation) NOEXCEPT;

}