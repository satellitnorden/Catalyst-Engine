#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

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