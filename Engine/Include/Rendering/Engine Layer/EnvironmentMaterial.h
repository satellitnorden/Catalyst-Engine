#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class EnvironmentMaterial final
{

public:

	//The diffuse texture.
	TextureCubeMapHandle diffuseTexture;

	//The diffuse irradiance texture.
	TextureCubeMapHandle diffuseIrradianceTexture;

};