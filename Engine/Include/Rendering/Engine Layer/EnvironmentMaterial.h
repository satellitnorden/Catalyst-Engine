#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class EnvironmentMaterial final
{

public:

	//The diffuse texture.
	TextureCubeMapHandle diffuseTexture;

	//The diffuse irradiance texture.
	TextureCubeMapHandle diffuseIrradianceTexture;

	/*
	*	Bool operator overload.
	*/
	operator bool() const NOEXCEPT
	{
		return static_cast<bool>(diffuseTexture) && static_cast<bool>(diffuseIrradianceTexture);
	}

};