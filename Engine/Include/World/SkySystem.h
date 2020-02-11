#pragma once

//Core.
#include<Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class SkySystem final
{

public:

	/*
	*	Post-initializes the sky system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Returns the sky texture.
	*/
	FORCE_INLINE NO_DISCARD const TextureCubeHandle GetSkyTexture() const NOEXCEPT
	{
		return _SkyTexture;
	}

private:

	//The sky texture.
	TextureCubeHandle _SkyTexture;

};