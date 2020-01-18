#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Pair.h>

//Terrain.
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>

class TerrainUpdate final
{

public:

	//The patch informations.
	DynamicArray<TerrainPatchInformation> _PatchInformations;

	//The patch render informations.
	DynamicArray<TerrainPatchRenderInformation> _PatchRenderInformations;

	/*
	*	Default constructor
	*/
	TerrainUpdate() NOEXCEPT
	{

	}

};