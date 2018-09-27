#if defined(CATALYST_ENABLE_OCEAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class OceanMaterial final
{

public:

	//The normal texture.
	Texture2DHandle _NormalTexture;

	//The foam texture.
	Texture2DHandle _FoamTexture;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

};
#endif