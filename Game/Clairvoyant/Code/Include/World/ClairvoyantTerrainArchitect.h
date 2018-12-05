#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/Vector3.h>
#include <Math/Vector4.h>

//World.
#include <World/ClairvoyantWorldConstants.h>

//Forward declarations.
class TerrainMaterial;
class TerrainProperties;

class ClairvoyantTerrainArchitect final
{

public:

	/*
	*	Initializes the Clairvoyant terrain architect.
	*/
	static void Initialize() NOEXCEPT;

	/*
	*	Generates the height.
	*/
	static void GenerateHeight(const TerrainProperties &properties, const Vector3<float> &position, float *const RESTRICT height) NOEXCEPT;

	/*
	*	Generates the material.
	*/
	static void GenerateMaterial(const TerrainProperties &properties, const Vector3<float> &worldPosition, const float height, const Vector3<float> &normal, Vector4<byte> *const RESTRICT albedo) NOEXCEPT;

private:

	//Enumeration covering all Clairvoyant terrain materials.
	enum class ClairvoyantTerrainMaterial : uint8
	{
		Grass,

		NumberOfClairvoyantTerrainMaterials
	};

	//Container for all Clairvoyant terrain materials.
	static StaticArray<const TerrainMaterial *RESTRICT, UNDERLYING(ClairvoyantTerrainMaterial::NumberOfClairvoyantTerrainMaterials)> _ClairvoyantTerrainMaterials;

	/*
	*	Returns whether or not a position is within the test area.
	*/
	static bool IsWithinTestArea(const Vector3<float> &worldPosition) NOEXCEPT;

	/*
	*	Returns the weight for the given material.
	*/
	static float GetMaterialWeight(const uint8 material, const float coordinateX, const float coordinateY, const StaticArray<float, UNDERLYING(ClairvoyantBiome::NumberOfClairvoyantBiomes)> &biomWeights, const Vector3<float> &normal) NOEXCEPT;

};