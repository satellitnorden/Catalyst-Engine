#pragma once

//Engine core.
#include <EngineCore.h>

//Rendering.
#include <RenderingCore.h>

class PhysicalMaterial final
{

public:

	//The albedo texture.
	Texture2DHandle albedoTexture;

	//The normal map texture.
	Texture2DHandle normalMapTexture;

	//The material properties texture.
	Texture2DHandle materialPropertiesTexture;

};