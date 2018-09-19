#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderingCore.h>

class EnvironmentMaterial final
{

public:

	//The diffuse texture.
	TextureCubeHandle _DiffuseTexture;

	//The diffuse irradiance texture.
	TextureCubeHandle _DiffuseIrradianceTexture;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

};