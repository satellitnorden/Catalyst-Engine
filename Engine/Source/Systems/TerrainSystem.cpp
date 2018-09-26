//Header file.
#include <Systems/TerrainSystem.h>

//Core.
#include <Core/General/CatalystProjectConfiguration.h>

//Singleton definition.
DEFINE_SINGLETON(TerrainSystem);

/*
*	Initializes the terrain system.
*/
void TerrainSystem::InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT
{
	//Copy over the relevant properties.
	_Properties._Height = configuration._Height;
	_Properties._PatchResolution = configuration._PatchResolution;
	_Properties._PatchSize = configuration._PatchSize;
}