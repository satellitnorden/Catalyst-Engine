#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>
#include <Rendering/Engine/CPUTexture2D.h>
#include <Rendering/Engine/RenderingCore.h>

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

	//The albedo texture.
	Texture2DHandle _AlbedoTexture;

	//The normal texture.
	Texture2DHandle _NormalTexture;

	//The material properties texture.
	Texture2DHandle _MaterialPropertiesTexture;

};