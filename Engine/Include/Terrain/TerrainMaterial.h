#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/Padding.h>

class TerrainMaterial final
{
public:

	//The index for the albedo texture in the global textures.
	uint32 _AlbedoTextureIndex;

	//The index for the normal map texture in the global textures.
	uint32 _NormalMapTextureIndex;

	//The index for the material properties texture in the global textures.
	uint32 _MaterialPropertiesTextureIndex;

	//Padding.
	Padding<4> _Padding;

};