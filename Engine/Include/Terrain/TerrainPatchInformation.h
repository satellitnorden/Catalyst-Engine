#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/GridPoint.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/TerrainMaterial.h>

//Terrain.
#include <Terrain/TerrainDisplacementInformation.h>

class TerrainPatchInformation final
{

public:

	//Denotes whether or not this patch is valid.
	std::atomic<bool> _Valid;

	//The grid point.
	GridPoint _GridPoint;

	//The axis-aligned bounding box for this terrain patch.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The material.
	TerrainMaterial _Material;

};