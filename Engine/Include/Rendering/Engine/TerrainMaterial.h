#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

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

	//The fourth layer albedo texture.
	Texture2DHandle fourthLayerAlbedo;

	//The fourth layer normal map.
	Texture2DHandle fourthLayerNormalMap;

	//The fourth layer material properties.
	Texture2DHandle fourthLayerMaterialProperties;

	//The fifth layer albedo texture.
	Texture2DHandle fifthLayerAlbedo;

	//The fifth layer normal map.
	Texture2DHandle fifthLayerNormalMap;

	//The fifth layer material properties.
	Texture2DHandle fifthLayerMaterialProperties;

};