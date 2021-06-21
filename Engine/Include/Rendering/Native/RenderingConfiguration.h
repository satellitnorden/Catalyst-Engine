#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class RenderingConfiguration final
{

public:

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
		SCREEN_SPACE,
		RAY_TRACED
	};

	//Enumeration covering all indirect lighting qualities.
	enum class IndirectLightingQuality : uint8
	{
		/*
		*	This setting will render the indirect lighting at half resolution.
		*/
		LOW,

		/*
		*	This setting will render the indirect lighting at full resolution.
		*/
		HIGH
	};

	//Enumeration covering all surface shadows modes.
	enum class SurfaceShadowsMode : uint8
	{
		NONE,
		RASTERIZED,
		RAY_TRACED
	};

	//Enumeration covering all volumetric shadows modes.
	enum class VolumetricShadowsMode : uint8
	{
		NONE,
		SCREEN_SPACE,
		RAY_TRACED
	};

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
	*	Sets the indirect lighting mode.
	*/
	FORCE_INLINE void SetIndirectLightingMode(const IndirectLightingMode value) NOEXCEPT
	{
		_IndirectLightingMode = value;
	}

	/*
	*	Returns the indirect lighting quality.
	*/
	FORCE_INLINE NO_DISCARD IndirectLightingQuality GetIndirectLightingQuality() const NOEXCEPT
	{
		return _IndirectLightingQuality;
	}

	/*
	*	Sets the indirect lighting quality.
	*/
	FORCE_INLINE void SetIndirectLightingQuality(const IndirectLightingQuality value) NOEXCEPT
	{
		_IndirectLightingQuality = value;
	}

	/*
	*	Returns the surface shadows mode.
	*/
	FORCE_INLINE NO_DISCARD SurfaceShadowsMode GetSurfaceShadowsMode() const NOEXCEPT
	{
		return _SurfaceShadowsMode;
	}

	/*
	*	Sets the surface shadows mode.
	*/
	FORCE_INLINE void SetSurfaceShadowsMode(const SurfaceShadowsMode value) NOEXCEPT
	{
		_SurfaceShadowsMode = value;
	}

	/*
	*	Returns the volumetric shadows mode.
	*/
	FORCE_INLINE NO_DISCARD VolumetricShadowsMode GetVolumetricShadowsMode() const NOEXCEPT
	{
		return _VolumetricShadowsMode;
	}

	/*
	*	Sets the volumetric shadows mode.
	*/
	FORCE_INLINE void SetVolumetricShadowsMode(const VolumetricShadowsMode value) NOEXCEPT
	{
		_VolumetricShadowsMode = value;
	}

	/*
	*	Returns the bloom intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetBloomIntensity() const NOEXCEPT
	{
		return _BloomIntensity;
	}

	/*
	*	Sets the bloom intensity.
	*/
	FORCE_INLINE void SetBloomIntensity(const float32 intensity) NOEXCEPT
	{
		_BloomIntensity = intensity;
	}

private:

	//The ambient occlusion mode.
	AmbientOcclusionMode _AmbientOcclusionMode{ AmbientOcclusionMode::SCREEN_SPACE };

	//The indirect lighting mode.
	IndirectLightingMode _IndirectLightingMode{ IndirectLightingMode::SCREEN_SPACE };

	//The indirect lighting quality.
	IndirectLightingQuality _IndirectLightingQuality{ IndirectLightingQuality::LOW };

	//The surface shadows mode.
	SurfaceShadowsMode _SurfaceShadowsMode{ SurfaceShadowsMode::RASTERIZED };

	//The volumetric shadows mode.
	VolumetricShadowsMode _VolumetricShadowsMode{ VolumetricShadowsMode::SCREEN_SPACE };

	//The bloom intensity.
	float32 _BloomIntensity{ 0.05f };

};