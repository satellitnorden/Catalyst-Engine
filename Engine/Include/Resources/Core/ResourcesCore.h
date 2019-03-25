#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	//Old resource types. Should probably be removed one after one soon...
	EnvironmentMaterial,
	GrassVegetationMaterial,
	GrassVegetationModel,
	OceanMaterial,
	ParticleMaterial,
	PhysicalMaterial,
	PhysicalModel,
	SoundBank,
	TreeVegetationMaterial,
	TreeVegetationModel,

	//New resource types. Yay!
	Model,

	NumberOfResourceTypes
};