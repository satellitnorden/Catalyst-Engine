#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/RenderingCore.h>

class TerrainPatchInformation final
{

public:

	//The axis-aligned bounding box for this terrain patch.
	AxisAlignedBoundingBox3D _AxisAlignedBoundingBox;

};