#pragma once

//Engine core.
#include <EngineCore.h>

//Enumeration covering all different resources types.
enum class ResourceType : uint8
{
	PhysicalMaterial,
	PhysicalModel,
	TerrainMaterial,
	WaterMaterial,
	NumberOfResourceTypes
};