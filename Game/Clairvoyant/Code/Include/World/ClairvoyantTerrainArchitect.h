#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Forward declarations.
class TerrainProperties;
class Vector3;
class Vector4;

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
	static void GenerateHeight(const TerrainProperties &properties, const Vector3 &position, float *const RESTRICT height) NOEXCEPT;

	/*
	*	Generates the material.
	*/
	static void GenerateMaterial(const TerrainProperties &properties, const Vector3 &position, uint8 *const RESTRICT material) NOEXCEPT;

private:

	/*
	*	Returns whether or not a position is within the test area.
	*/
	static bool IsWithinTestArea(const Vector3 &worldPosition) NOEXCEPT;

};