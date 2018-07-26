#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	EnvironmentMaterial,
	ParticleMaterial,
	PhysicalMaterial,
	PhysicalModel,
	SoundBank,
	TerrainMaterial,
	VegetationMaterial,
	OceanMaterial,
	NumberOfResourceTypes
};