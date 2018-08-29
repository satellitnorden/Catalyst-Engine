#pragma once

//Core.
#include <Core/General/EngineCore.h>

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