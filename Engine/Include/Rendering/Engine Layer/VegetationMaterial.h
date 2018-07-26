#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

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