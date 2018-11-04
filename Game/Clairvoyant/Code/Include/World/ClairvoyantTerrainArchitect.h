#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class TerrainMaterial;
class TerrainProperties;
class Vector3;
class Vector4;

class ClairvoyantTerrainArchitect final
{

public:

	/*
	*	Generates the height.
	*/
	static void GenerateHeight(const TerrainProperties &properties, const Vector3 &worldPosition, float *const RESTRICT height) NOEXCEPT;

	/*
	*	Generates the layer weights.
	*/
	static void GenerateLayerWeights(const TerrainProperties &properties, const Vector3 &worldPosition, Vector4 *const RESTRICT layerWeights) NOEXCEPT;

	/*
	*	Generates the patch properties.
	*/
	static void GeneratePatchProperties(const TerrainProperties &properties, const Vector3 &worldPosition, TerrainMaterial *const RESTRICT) NOEXCEPT;

};