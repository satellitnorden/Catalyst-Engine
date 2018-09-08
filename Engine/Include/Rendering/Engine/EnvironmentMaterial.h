#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class EnvironmentMaterial final
{

public:

	//The diffuse texture.
	TextureCubeMapHandle _DiffuseTexture;

	//The diffuse irradiance texture.
	TextureCubeMapHandle _DiffuseIrradianceTexture;

};