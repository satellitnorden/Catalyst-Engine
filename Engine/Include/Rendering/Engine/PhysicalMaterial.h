#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class PhysicalMaterial final
{

public:

	//The albedo texture.
	Texture2DHandle _AlbedoTexture;

	//The normal map texture.
	Texture2DHandle _NormalMapTexture;

	//The material properties texture.
	Texture2DHandle _MaterialPropertiesTexture;

	//The index for the albedo texture in the global textures.
	uint32 _AlbedoTextureIndex;

	//The index for the normal map texture in the global textures.
	uint32 _NormalMapTextureIndex;

	//The index for the material properties texture in the global textures.
	uint32 _MaterialPropertiesTextureIndex;

};