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

};