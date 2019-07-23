#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/RenderingCore.h>

class TerrainPatchInformation final
{

public:

	//The identifier.
	uint64 _Identifier;

	//Denotes whether or not this patch is valid.
	bool _Valid;

	//The axis-aligned bounding box for this terrain patch.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

	//The height texture.
	Texture2DHandle _HeightTexture;

	//The normal texture.
	Texture2DHandle _NormalTexture;

	//The material texture.
	Texture2DHandle _MaterialTexture;

};