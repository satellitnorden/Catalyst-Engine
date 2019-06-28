#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class RenderingConfigurationManager final
{

public:

	//Enumeration covering all anti aliasing modes.
	enum class AntiAliasingMode : uint8
	{
		None,
		FastApproximate,
		Temporal,
		FastApproximateAndTemporal
	};

	//Enumeration covering all diffuse irradiance modes.
	enum class DiffuseIrradianceMode : uint8
	{
		Simple,
		RayTraced
	};

	//Enumeration covering all specular irradiance modes.
	enum class SpecularIrradianceMode : uint8
	{
		None,
		ScreenSpace,
		RayTraced
	};

	//Enumeration covering all shadows modes.
	enum class ShadowsMode : uint8
	{
		None,
		RayTraced
	};

	//Enumeration covering all volumetric lighting modes.
	enum class VolumetricLightingMode : uint8
	{
		None,
		RayTraced
	};

	//Singleton declaration.
	DECLARE_SINGLETON(RenderingConfigurationManager);

	/*
	*	Default constructor.
	*/
	RenderingConfigurationManager() NOEXCEPT
	{
	
	}

	/*
	*	Returns the anti aliasing mode.
	*/
	AntiAliasingMode GetAntiAliasingMode() const NOEXCEPT
	{
		return _AntiAliasingMode;
	}

	/*
	*	Sets the anti aliasing mode.
	*/
	void SetAntiAliasingMode(const AntiAliasingMode newMode) NOEXCEPT
	{
		_AntiAliasingMode = newMode;
	}

	/*
	*	Returns the diffuse irradiance mode.
	*/
	DiffuseIrradianceMode GetDiffuseIrradianceMode() const NOEXCEPT
	{
		return _DiffuseIrradianceMode;
	}

	/*
	*	Returns the specular irradiance mode.
	*/
	SpecularIrradianceMode GetSpecularIrradianceMode() const NOEXCEPT
	{
		return _SpecularIrradianceMode;
	}

	/*
	*	Returns the shadows mode.
	*/
	ShadowsMode GetShadowsMode() const NOEXCEPT
	{
		return _ShadowsMode;
	}

	/*
	*	Returns the volumetric lighting mode.
	*/
	VolumetricLightingMode GetVolumetricLightingMode() const NOEXCEPT
	{
		return _VolumetricLightingMode;
	}

	/*
	*	Returns the ambient illumination intensity.
	*/
	FORCE_INLINE NO_DISCARD float GetAmbientIlluminationIntensity() const NOEXCEPT
	{
		return _AmbientIlluminationIntensity;
	}

	/*
	*	Sets the ambient illumination intensity.
	*/
	FORCE_INLINE void SetAmbientIlluminationIntensity(const float intensity) NOEXCEPT
	{
		_AmbientIlluminationIntensity = intensity;
	}

	/*
	*	Returns the bloom intensity.
	*/
	FORCE_INLINE NO_DISCARD float GetBloomIntensity() const NOEXCEPT
	{
		return _BloomIntensity;
	}

	/*
	*	Sets the bloom intensity.
	*/
	FORCE_INLINE void SetBloomIntensity(const float intensity) NOEXCEPT
	{
		_BloomIntensity = intensity;
	}

	/*
	*	Returns the chromatic aberration intensity.
	*/
	FORCE_INLINE NO_DISCARD float GetChromaticAberrationIntensity() const NOEXCEPT
	{
		return _ChromaticAberrationIntensity;
	}

	/*
	*	Sets the chromatic aberration intensity.
	*/
	FORCE_INLINE void SetChromaticAberrationIntensity(const float intensity) NOEXCEPT
	{
		_ChromaticAberrationIntensity = intensity;
	}

	/*
	*	Returns the volumetric lighting intensity.
	*/
	FORCE_INLINE NO_DISCARD float GetVolumetricLightingIntensity() const NOEXCEPT
	{
		return _VolumetricLightingIntensity;
	}

	/*
	*	Sets the volumetric lighting intensity.
	*/
	FORCE_INLINE void SetVolumetricLightingIntensity(const float intensity) NOEXCEPT
	{
		_VolumetricLightingIntensity = intensity;
	}

private:

	//The anti aliasing mode.
	AntiAliasingMode _AntiAliasingMode{ AntiAliasingMode::FastApproximateAndTemporal };

	//The diffuse irradiance mode.
	DiffuseIrradianceMode _DiffuseIrradianceMode{ DiffuseIrradianceMode::Simple };

	//The specular irradiance mode.
	SpecularIrradianceMode _SpecularIrradianceMode{ SpecularIrradianceMode::ScreenSpace };

	//The shadows mode.
	ShadowsMode _ShadowsMode{ ShadowsMode::RayTraced };

	//The volumetric lighting mode.
	VolumetricLightingMode _VolumetricLightingMode{ VolumetricLightingMode::RayTraced };

	//The ambient illumination intensity.
	float _AmbientIlluminationIntensity{ 0.1f };

	//The bloom intensity.
	float _BloomIntensity{ 1.0f };

	//The chromatic aberration intensity.
	float _ChromaticAberrationIntensity{ 0.001f };

	//The volumetric lighting intensity.
	float _VolumetricLightingIntensity{ 1.0f };

};