#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	//EnvironmentMaterial,
	PhysicalMaterial,
	PhysicalModel,
	TerrainMaterial,
	WaterMaterial,
	NumberOfResourceTypes
};

//Type aliases.
using ResourceID = uint64;