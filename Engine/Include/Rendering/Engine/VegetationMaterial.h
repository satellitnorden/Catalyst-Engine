#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class VegetationMaterial final
{

public:

	//The mask texture.
	Texture2DHandle maskTexture;

	//The albedo texture.
	Texture2DHandle albedoTexture;

	//The normal map texture.
	Texture2DHandle normalMapTexture;

	//The properties texture.
	Texture2DHandle propertiesTexture;

};