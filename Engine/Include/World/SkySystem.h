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
		const_cast<SkySystem *const RESTRICT>(this)->InitializeSkyTexture();

		return _SkyTexture;
	}

	/*
	*	Returns the sky texture resolution.
	*/
	NO_DISCARD uint32 GetSkyTextureResolution() const NOEXCEPT;

private:

	//Denotes if the sky texture is initialized.
	bool _SkyTextureInitialized{ false };

	//The sky texture.
	TextureCubeHandle _SkyTexture;

	/*
	*	Initializes the sky texture.
	*/
	void InitializeSkyTexture() NOEXCEPT;

};