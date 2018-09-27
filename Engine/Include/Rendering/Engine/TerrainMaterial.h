#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class TerrainMaterial final
{

public:

	//The first layer albedo texture.
	Texture2DHandle _FirstLayerAlbedo;

	//The first layer normal map.
	Texture2DHandle _FirstLayerNormalMap;

	//The first layer material properties.
	Texture2DHandle _FirstLayerMaterialProperties;

	//The second layer albedo texture.
	Texture2DHandle _SecondLayerAlbedo;

	//The second layer normal map.
	Texture2DHandle _SecondLayerNormalMap;

	//The second layer material properties.
	Texture2DHandle _SecondLayerMaterialProperties;

	//The third layer albedo texture.
	Texture2DHandle _ThirdLayerAlbedo;

	//The third layer normal map.
	Texture2DHandle _ThirdLayerNormalMap;

	//The third layer material properties.
	Texture2DHandle _ThirdLayerMaterialProperties;

	//The fourth layer albedo texture.
	Texture2DHandle _FourthLayerAlbedo;

	//The fourth layer normal map.
	Texture2DHandle _FourthLayerNormalMap;

	//The fourth layer material properties.
	Texture2DHandle _FourthLayerMaterialProperties;

	//The fifth layer albedo texture.
	Texture2DHandle _FifthLayerAlbedo;

	//The fifth layer normal map.
	Texture2DHandle _FifthLayerNormalMap;

	//The fifth layer material properties.
	Texture2DHandle _FifthLayerMaterialProperties;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

};