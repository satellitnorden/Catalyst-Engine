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

	//Enumeration covering all surface shadows modes.
	enum class SurfaceShadowsMode : uint8
	{
		NONE,
		RAY_TRACED
	};

	//Enumeration covering all volumetric shadows modes.
	enum class VolumetricShadowsMode : uint8
	{
		NONE,
		SCREEN_SPACE,
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
	*	Sets the ambient occlusion mode.
	*/
	FORCE_INLINE void SetAmbientOcclusionMode(const AmbientOcclusionMode value) NOEXCEPT
	{
		_AmbientOcclusionMode = value;
	}

	/*
	*	Returns the indirect lighting mode.
	*/
	FORCE_INLINE NO_DISCARD IndirectLightingMode GetIndirectLightingMode() const NOEXCEPT
	{
		return _IndirectLightingMode;
	}

	/*
	*	Returns the surface shadows mode.
	*/
	FORCE_INLINE NO_DISCARD SurfaceShadowsMode GetSurfaceShadowsMode() const NOEXCEPT
	{
		return _SurfaceShadowsMode;
	}

	/*
	*	Returns the volumetric shadows mode.
	*/
	FORCE_INLINE NO_DISCARD VolumetricShadowsMode GetVolumetricShadowsMode() const NOEXCEPT
	{
		return _VolumetricShadowsMode;
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

private:

	//The rendering path.
	RenderingPath _RenderingPath{ RenderingPath::MAIN };

	//The ambient occlusion mode.
	AmbientOcclusionMode _AmbientOcclusionMode{ AmbientOcclusionMode::SCREEN_SPACE };

	//The indirect lighting mode.
	IndirectLightingMode _IndirectLightingMode{ IndirectLightingMode::NONE };

	//The surface shadows mode.
	SurfaceShadowsMode _SurfaceShadowsMode{ SurfaceShadowsMode::NONE };

	//The volumetric shadows mode.
	VolumetricShadowsMode _VolumetricShadowsMode{ VolumetricShadowsMode::SCREEN_SPACE };

	//The bloom intensity.
	float _BloomIntensity{ 0.1f };

};