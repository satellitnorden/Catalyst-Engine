#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class OceanMaterial final
{

public:

	//The normal map texture.
	Texture2DHandle normalMapTexture;

	/*
	*	Bool operator overload.
	*/
	operator bool() const NOEXCEPT
	{
		return static_cast<bool>(normalMapTexture);
	}

};