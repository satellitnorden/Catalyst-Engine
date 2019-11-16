#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class RenderingConfigurationManager final
{

public:

	//Enumeration covering all ambient occlusion modes.
	enum class AmbientOcclusionMode : uint8
	{
		None,
		ScreenSpace
	};

	//Enumeration covering all motion blur modes.
	enum class MotionBlurMode : uint8
	{
		None,
		Half,
		Full
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

	//Singleton declaration.
	DECLARE_SINGLETON(RenderingConfigurationManager);

	/*
	*	Default constructor.
	*/
	RenderingConfigurationManager() NOEXCEPT
	{
	
	}

	/*
	*	Returns the ambient occlusion mode.
	*/
	AmbientOcclusionMode GetAmbientOcclusionMode() const NOEXCEPT
	{
		return _AmbientOcclusionMode;
	}

	/*
	*	Returns the motion blur mode.
	*/
	MotionBlurMode GetMotionBlurMode() const NOEXCEPT
	{
		return _MotionBlurMode;
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

	/*
	*	Returns the depth of field focus distance.
	*/
	FORCE_INLINE NO_DISCARD float GetDepthOfFieldFocusDistance() const NOEXCEPT
	{
		return _DepthOfFieldFocusDistance;
	}

	/*
	*	Sets the depth of field focus distance.
	*/
	FORCE_INLINE void SetDepthOfFieldFocusDistance(const float distance) NOEXCEPT
	{
		_DepthOfFieldFocusDistance = distance;
	}

private:

	//The ambient occlusion mode.
	AmbientOcclusionMode _AmbientOcclusionMode{ AmbientOcclusionMode::ScreenSpace };

	//The motion blur mode mode.
	MotionBlurMode _MotionBlurMode{ MotionBlurMode::Full };

	//The specular irradiance mode.
	SpecularIrradianceMode _SpecularIrradianceMode{ SpecularIrradianceMode::ScreenSpace };

	//The shadows mode.
	ShadowsMode _ShadowsMode{ ShadowsMode::None };

	//The bloom intensity.
	float _BloomIntensity{ 0.1f };

	//The chromatic aberration intensity.
	float _ChromaticAberrationIntensity{ 0.0025f };

	//The volumetric lighting intensity.
	float _VolumetricLightingIntensity{ 1.0f };

	//The depth of field focus distance.
	float _DepthOfFieldFocusDistance{ 0.0f };

};