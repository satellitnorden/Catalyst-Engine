#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Optional.h>

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

	/*
	*	Returns the sky texture with the given index.
	*/
	FORCE_INLINE NO_DISCARD const TextureCubeHandle GetSkyTexture(const uint32 index) const NOEXCEPT
	{
		const_cast<SkySystem *const RESTRICT>(this)->InitializeSkyTextures();

		return _SkyTextures[index];
	}

	/*
	*	Returns the sky texture override.
	*/
	FORCE_INLINE NO_DISCARD Optional<TextureCubeHandle> GetSkyTextureOverride() const NOEXCEPT
	{
		return _SkyTextureOverride;
	}

	/*
	*	Sets the sky texture override.
	*/
	FORCE_INLINE void SetSkyTextureOverride(const TextureCubeHandle sky_texture) NOEXCEPT
	{
		_SkyTextureOverride = sky_texture;
	}

	/*
	*	Returns the current sky gradient.
	*/
	NO_DISCARD SkyGradient GetCurrentSkyGradient() const NOEXCEPT;

	/*
	*	Returns the current star intensity.
	*/
	NO_DISCARD float GetCurrentStarIntensity() const NOEXCEPT;

	/*
	*	Returns the cloud density.
	*/
	FORCE_INLINE NO_DISCARD float GetCloudDensity() const NOEXCEPT
	{
		return _CloudDensity;
	}

	/*
	*	Sets the sky gradient override.
	*/
	FORCE_INLINE void SetSkyGradientOverride(const SkyGradient& sky_gradient) NOEXCEPT
	{
		_SkyGradientOverride = sky_gradient;
	}

	/*
	*	Sets the star strength override.
	*/
	FORCE_INLINE void SetStarStrengthOverride(const float32 star_strength) NOEXCEPT
	{
		_StarStrengthOverride = star_strength;
	}

	/*
	*	Sets the cloud density.
	*/
	FORCE_INLINE void SetCloudDensity(const float cloud_density) NOEXCEPT
	{
		_CloudDensity = cloud_density;
	}

private:

	//Denotes if the sky textures are initialized.
	bool _SkyTexturesInitialized{ false };

	//The sky intensity.
	float32 _SkyIntensity{ 1.0f };

	//The sky textures.
	StaticArray<TextureCubeHandle, CatalystShaderConstants::NUMBER_OF_SKY_TEXTURES> _SkyTextures;

	//The sky texture override.
	Optional<TextureCubeHandle> _SkyTextureOverride;

	//The sky gradient override.
	Optional<SkyGradient> _SkyGradientOverride;

	//The star strength override.
	Optional<float32> _StarStrengthOverride;

	//The cloud density.
	float _CloudDensity{ 0.5f };

	/*
	*	Initializes the sky textures.
	*/
	void InitializeSkyTextures() NOEXCEPT;

};