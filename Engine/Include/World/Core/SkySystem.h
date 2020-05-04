#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Optional.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/TextureCubeResource.h>

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
	FORCE_INLINE NO_DISCARD const ResourcePointer<TextureCubeResource> GetSkyTexture() const NOEXCEPT
	{
		return _SkyTexture;
	}

	/*
	*	Sets the sky texture.
	*/
	FORCE_INLINE void SetSkyTexture(const ResourcePointer<TextureCubeResource> value) NOEXCEPT
	{
		_SkyTexture = value;
	}

	/*
	*	Returns the sky intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetSkyIntensity() const NOEXCEPT
	{
		return _SkyIntensity;
	}

	/*
	*	Sets the sky intensity.
	*/
	FORCE_INLINE void SetSkyIntensity(const float32 value) NOEXCEPT
	{
		_SkyIntensity = value;
	}

private:

	//The sky texture.
	ResourcePointer<TextureCubeResource> _SkyTexture;

	//The sky intensity.
	float32 _SkyIntensity{ 1.0f };

};