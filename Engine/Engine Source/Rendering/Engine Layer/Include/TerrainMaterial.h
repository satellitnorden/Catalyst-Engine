#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>

class TerrainMaterial final
{

public:

	//The first layer albedo texture.
	Texture2DHandle firstLayerAlbedo;

	//The first layer normal map.
	Texture2DHandle firstLayerNormalMap;

	//The first layer material properties.
	Texture2DHandle firstLayerMaterialProperties;

	//The second layer albedo texture.
	Texture2DHandle secondLayerAlbedo;

	//The second layer normal map.
	Texture2DHandle secondLayerNormalMap;

	//The second layer material properties.
	Texture2DHandle secondLayerMaterialProperties;

	//The third layer albedo texture.
	Texture2DHandle thirdLayerAlbedo;

	//The third layer normal map.
	Texture2DHandle thirdLayerNormalMap;

	//The third layer material properties.
	Texture2DHandle thirdLayerMaterialProperties;

};