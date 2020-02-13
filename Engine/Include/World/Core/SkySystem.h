#pragma once

//Core.
#include<Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Sky/SkyGradient.h>

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
	FORCE_INLINE NO_DISCARD const TextureCubeHandle GetSkyTexture(const uint32 index) const NOEXCEPT
	{
		const_cast<SkySystem *const RESTRICT>(this)->InitializeSkyTextures();

		return _SkyTextures[index];
	}

	/*
	*	Returns the current sky gradient.
	*/
	NO_DISCARD SkyGradient GetCurrentSkyGradient() NOEXCEPT;

private:

	//Denotes if the sky textures are initialized.
	bool _SkyTexturesInitialized{ false };

	//The sky textures.
	StaticArray<TextureCubeHandle, CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES> _SkyTextures;

	/*
	*	Initializes the sky textures.
	*/
	void InitializeSkyTextures() NOEXCEPT;

};