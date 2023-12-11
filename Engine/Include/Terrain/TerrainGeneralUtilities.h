#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Terrain.
#include <Terrain/TerrainVertex.h>

namespace TerrainGeneralUtilities
{

	/*
	*	Generates the vertices and indices for a terrain plane.
	*/
	void GenerateTerrainPlane(const uint32 resolution, DynamicArray<TerrainVertex> *const RESTRICT vertices, DynamicArray<uint32> *const RESTRICT indices) NOEXCEPT;

}