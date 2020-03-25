#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class RenderingConfigurationManager final
{

public:

	//Enumeration covering all rendering paths.
	enum class RenderingPath : uint8
	{
		MAIN,
		PATH_TRACING
	};

	//Enumeration covering all ambient occlusion modes.
	enum class AmbientOcclusionMode : uint8
	{
		NONE,
		SCREEN_SPACE,
		RAY_TRACED
	};

	//Enumeration covering all indirect lighting modes.
	enum class IndirectLightingMode : uint8
	{
		NONE,
		SCREEN_SPACE
	};

	//Enumeration covering all motion blur modes.
	enum class MotionBlurMode : uint8
	{
		None,
		Half,
		Full
	};

	//Enumeration covering all shadows modes.
	enum class ShadowsMode : uint8
	{
		NONE,
		RAY_TRACED
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
	*	Returns the rendering path.
	*/
	FORCE_INLINE NO_DISCARD RenderingPath GetRenderingPath() const NOEXCEPT
	{
		return _RenderingPath;
	}

	/*
	*	Returns the ambient occlusion mode.
	*/
	FORCE_INLINE NO_DISCARD AmbientOcclusionMode GetAmbientOcclusionMode() const NOEXCEPT
	{
		return _AmbientOcclusionMode;
	}

	/*
	*	Returns the indirect lighting mode.
	*/
	FORCE_INLINE NO_DISCARD IndirectLightingMode GetIndirectLightingMode() const NOEXCEPT
	{
		return _IndirectLightingMode;
	}

	/*
	*	Returns the motion blur mode.
	*/
	FORCE_INLINE NO_DISCARD MotionBlurMode GetMotionBlurMode() const NOEXCEPT
	{
		return _MotionBlurMode;
	}

	/*
	*	Returns the shadows mode.
	*/
	FORCE_INLINE NO_DISCARD ShadowsMode GetShadowsMode() const NOEXCEPT
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

	//The rendering path.
	RenderingPath _RenderingPath{ RenderingPath::MAIN };

	//The ambient occlusion mode.
	AmbientOcclusionMode _AmbientOcclusionMode{ AmbientOcclusionMode::RAY_TRACED };

	//The indirect lighting mode.
	IndirectLightingMode _IndirectLightingMode{ IndirectLightingMode::NONE };

	//The motion blur mode.
	MotionBlurMode _MotionBlurMode{ MotionBlurMode::Full };

	//The shadows mode.
	ShadowsMode _ShadowsMode{ ShadowsMode::RAY_TRACED };

	//The bloom intensity.
	float _BloomIntensity{ 0.1f };

	//The chromatic aberration intensity.
	float _ChromaticAberrationIntensity{ 0.0025f };

	//The depth of field focus distance.
	float _DepthOfFieldFocusDistance{ 10.0f };

};