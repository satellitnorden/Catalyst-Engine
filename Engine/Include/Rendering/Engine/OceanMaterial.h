#if !defined(CATALYST_DISABLE_OCEAN)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class OceanMaterial final
{

public:

	//The normal map texture.
	Texture2DHandle _NormalMapTexture;

};
#endif