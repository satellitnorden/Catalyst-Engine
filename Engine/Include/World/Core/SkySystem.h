#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Optional.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/TextureCubeAsset.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//World.
#include <World/Sky/AtmosphericScatteringProperties.h>
#include <World/Sky/SkyGradient.h>

class SkySystem final
{

public:

	//Enumeration covering all sky modes.
	enum class SkyMode : uint8
	{
		ATMOSPHERIC_SCATTERING,
		GRADIENT,
		TEXTURE
	};

	/*
	*	Post-initializes the sky system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Returns whether or not sky is enabled.
	*/
	FORCE_INLINE NO_DISCARD bool GetSkyEnabled() const NOEXCEPT
	{
		return _SkyEnabled;
	}

	/*
	*	Sets whether or not sky is enabled.
	*/
	FORCE_INLINE void SetSkyEnabled(const bool value) NOEXCEPT
	{
		_SkyEnabled = value;
	}

	/*
	*	Returns the sky mode.
	*/
	FORCE_INLINE NO_DISCARD SkyMode GetSkyMode() const NOEXCEPT
	{
		return _SkyMode;
	}

	/*
	*	Returns the atmospheric scattering properties.
	*/
	FORCE_INLINE NO_DISCARD const AtmosphericScatteringProperties &GetAtmosphericScatteringProperties() const NOEXCEPT
	{
		return _AtmosphericScatteringProperties;
	}

	/*
	*	Sets the atmospheric scattering properties.
	*/
	FORCE_INLINE void SetAtmosphericScatteringProperties(const AtmosphericScatteringProperties &value) NOEXCEPT
	{
		_SkyMode = SkyMode::ATMOSPHERIC_SCATTERING;
		_AtmosphericScatteringProperties = value;
	}

	/*
	*	Returns the sky gradient.
	*/
	FORCE_INLINE NO_DISCARD const SkyGradient &GetSkyGradient() const NOEXCEPT
	{
		return _SkyGradient;
	}

	/*
	*	Sets the sky gradient.
	*/
	FORCE_INLINE void SetSkyGradient(const SkyGradient &value) NOEXCEPT
	{
		_SkyMode = SkyMode::GRADIENT;
		_SkyGradient = value;
	}

	/*
	*	Returns the sky texture.
	*/
	FORCE_INLINE NO_DISCARD const AssetPointer<TextureCubeAsset> GetSkyTexture() const NOEXCEPT
	{
		return _SkyTexture;
	}

	/*
	*	Sets the sky texture.
	*/
	FORCE_INLINE void SetSkyTexture(const AssetPointer<TextureCubeAsset> value) NOEXCEPT
	{
		_SkyMode = SkyMode::TEXTURE;
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

	/*
	*	Returns the star intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetStarIntensity() const NOEXCEPT
	{
		return _StarIntensity;
	}

	/*
	*	Sets the star intensity.
	*/
	FORCE_INLINE void SetStarIntensity(const float32 value) NOEXCEPT
	{
		_StarIntensity = value;
	}

	/*
	*	Returns the cloud thickness.
	*/
	FORCE_INLINE NO_DISCARD float32 GetCloudThickness() const NOEXCEPT
	{
		return _CloudThickness;
	}

	/*
	*	Sets the cloud thickness.
	*/
	FORCE_INLINE void SetCloudThickness(const float32 value) NOEXCEPT
	{
		_CloudThickness = value;
	}

private:

	//Denotes whether or not sky is enabled.
	bool _SkyEnabled{ true };

	//The sky mode.
	SkyMode _SkyMode{ SkyMode::TEXTURE };

	//The atmospheric scattering properties.
	AtmosphericScatteringProperties _AtmosphericScatteringProperties;

	//The sky gradient.
	SkyGradient _SkyGradient;

	//The sky texture.
	AssetPointer<TextureCubeAsset> _SkyTexture;

	//The sky intensity.
	float32 _SkyIntensity{ 1.0f };

	//The star intensity.
	float32 _StarIntensity{ 0.0f };

	//The cloud thickness.
	float32 _CloudThickness{ 0.0f };

};