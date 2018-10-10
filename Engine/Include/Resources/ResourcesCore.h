#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	EnvironmentMaterial,
	GrassMaterial,
	GrassModel,
#if defined(CATALYST_ENABLE_OCEAN)
	OceanMaterial,
#endif
	ParticleMaterial,
	PhysicalMaterial,
	PhysicalModel,
	TerrainMaterial,

	NumberOfResourceTypes
};