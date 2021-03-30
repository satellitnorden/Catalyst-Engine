//Header file.
#include <World/Core/VegetationSystem.h>

/*
*	Enables procedural grass.
*/
void VegetationSystem::EnableProceduralGrass(const ProceduralGrassProperties& properties) NOEXCEPT
{
	//Set the procedural grass properties.
	_ProceduralGrassProperties = properties;
}