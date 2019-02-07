#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TreeVegetationMaterial final
{

public:

	//The crown mask texture.
	Texture2DHandle _CrownMaskTexture;

	//The crown albedo texture.
	Texture2DHandle _CrownAlbedoTexture;

	//The crown normal map texture.
	Texture2DHandle _CrownNormalMapTexture;

	//The impostor mask texture.
	Texture2DHandle _ImpostorMaskTexture;

	//The impostor albedo texture.
	Texture2DHandle _ImpostorAlbedoTexture;

	//The trunk albedo texture.
	Texture2DHandle _TrunkAlbedoTexture;

	//The trunk normal map texture.
	Texture2DHandle _TrunkNormalMapTexture;

	//The trunk material properties texture.
	Texture2DHandle _TrunkMaterialPropertiesTexture;

	//The index for the crown mask texture in the global textures.
	uint32 _CrownMaskTextureIndex;

	//The index for the crown albedo texture in the global textures.
	uint32 _CrownAlbedoTextureIndex;

	//The index for the crown normal map texture in the global textures.
	uint32 _CrownNormalMapTextureIndex;

	//The index for the impostor mask texture in the global textures.
	uint32 _ImpostorMaskTextureIndex;

	//The index for the impostor albedo texture in the global textures.
	uint32 _ImpostorAlbedoTextureIndex;

	//The index for the trunk albedo texture in the global textures.
	uint32 _TrunkAlbedoTextureIndex;

	//The index for the trunk normal map texture in the global textures.
	uint32 _TrunkNormalMapTextureIndex;

	//The index for the trunk material properties texture in the global textures.
	uint32 _TrunkMaterialPropertiesTextureIndex;

};