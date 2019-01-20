#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TreeVegetationMaterial final
{

public:

	//The trunk albedo texture.
	Texture2DHandle _TrunkAlbedoTexture;

	//The trunk normal map texture.
	Texture2DHandle _TrunkNormalMapTexture;

	//The trunk material properties texture.
	Texture2DHandle _TrunkMaterialPropertiesTexture;

	//The index for the trunk albedo texture in the global textures.
	uint32 _TrunkAlbedoTextureIndex;

	//The index for the trunk normal map texture in the global textures.
	uint32 _TrunkNormalMapTextureIndex;

	//The index for the trunk material properties texture in the global textures.
	uint32 _TrunkMaterialPropertiesTextureIndex;

};