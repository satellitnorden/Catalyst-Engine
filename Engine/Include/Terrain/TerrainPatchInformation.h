#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/GridPoint2.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/TerrainMaterial.h>

class TerrainPatchInformation final
{

public:

	//The identifier.
	uint64 _Identifier;

	//Denotes whether or not this patch is valid.
	bool _Valid;

	//The grid point.
	GridPoint2 _GridPoint;

	//The axis-aligned bounding box for this terrain patch.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The material.
	TerrainMaterial _Material;

	//The height texture.
	Texture2DHandle _HeightTexture;

	//The normal texture.
	Texture2DHandle _NormalTexture;

	//The layer weights texture.
	Texture2DHandle _LayerWeightsTexture;

};