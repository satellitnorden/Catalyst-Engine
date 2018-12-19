#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	EnvironmentMaterial,
	GrassMaterial,
	GrassVegetationModel,
	OceanMaterial,
	ParticleMaterial,
	PhysicalMaterial,
	PhysicalModel,

	NumberOfResourceTypes
};