#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class EnvironmentMaterial final
{

public:

	//The diffuse texture.
	TextureCubeMapHandle diffuseTexture;

	//The diffuse irradiance texture.
	TextureCubeMapHandle diffuseIrradianceTexture;

};