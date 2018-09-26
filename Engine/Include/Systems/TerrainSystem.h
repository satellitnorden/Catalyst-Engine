#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Pointers/UniquePointer.h>

//Terrain.
#include <Terrain/TerrainPatch.h>
#include <Terrain/TerrainProperties.h>

//Forward declarations.
class CatalystProjectTerrainConfiguration;

class TerrainSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TerrainSystem);

	/*
	*	Default constructor.
	*/
	TerrainSystem() NOEXCEPT { }

	/*
	*	Initializes the terrain system.
	*/
	void InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT;

private:

	//The properties.
	TerrainProperties _Properties;

	//The patches.
	DynamicArray<TerrainPatch> _Patches;

};