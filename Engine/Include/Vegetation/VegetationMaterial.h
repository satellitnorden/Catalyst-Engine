#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationMaterial final
{

public:

	//The mask texture.
	Texture2DHandle _MaskTexture;

	//The albedo texture.
	Texture2DHandle _AlbedoTexture;

	//The normal map texture.
	Texture2DHandle _NormalMapTexture;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

};