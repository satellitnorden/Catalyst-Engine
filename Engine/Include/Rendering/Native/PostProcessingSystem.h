#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Optional.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/Texture2DResource.h>

class PostProcessingSystem final
{

public:

	/*
	*	Returns the brightness.
	*/
	FORCE_INLINE NO_DISCARD float32 GetBrightness() const NOEXCEPT
	{
		return _Brightness;
	}

	/*
	*	Sets the brightness.
	*/
	FORCE_INLINE void SetBrightness(const float32 value) NOEXCEPT
	{
		_Brightness = value;
	}

	/*
	*	Returns the contrast.
	*/
	FORCE_INLINE NO_DISCARD float32 GetContrast() const NOEXCEPT
	{
		return _Contrast;
	}

	/*
	*	Sets the contrast.
	*/
	FORCE_INLINE void SetContrast(const float32 value) NOEXCEPT
	{
		_Contrast = value;
	}

	/*
	*	Returns the chromatic aberration intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetChromaticAberrationIntensity() const NOEXCEPT
	{
		return _ChromaticAberrationIntensity;
	}

	/*
	*	Sets the chromatic aberration intensity.
	*/
	FORCE_INLINE void SetChromaticAberrationIntensity(const float32 value) NOEXCEPT
	{
		_ChromaticAberrationIntensity = value;
	}

	/*
	*	Returns the depth of field focus distance.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDepthOfFieldFocusDistance() const NOEXCEPT
	{
		return _DepthOfFieldFocusDistance;
	}

	/*
	*	Sets the depth of field focus distance.
	*/
	FORCE_INLINE void SetDepthOfFieldFocusDistance(const float32 value) NOEXCEPT
	{
		_DepthOfFieldFocusDistance = value;
	}

	/*
	*	Returns the depth of field size.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDepthOfFieldSize() const NOEXCEPT
	{
		return _DepthOfFieldSize;
	}

	/*
	*	Sets the depth of field size.
	*/
	FORCE_INLINE void SetDepthOfFieldSize(const float32 value) NOEXCEPT
	{
		_DepthOfFieldSize = value;
	}

	/*
	*	Returns the exposure.
	*/
	FORCE_INLINE NO_DISCARD float32 GetExposure() const NOEXCEPT
	{
		return _Exposure;
	}

	/*
	*	Sets the exposure.
	*/
	FORCE_INLINE void SetExposure(const float32 value) NOEXCEPT
	{
		_Exposure = value;
	}

	/*
	*	Returns the film grain intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetFilmGrainIntensity() const NOEXCEPT
	{
		return _FilmGrainIntensity;
	}

	/*
	*	Sets the film grain intensity.
	*/
	FORCE_INLINE void SetFilmGrainIntensity(const float32 value) NOEXCEPT
	{
		_FilmGrainIntensity = value;
	}

	/*
	*	Returns the horizontal border.
	*/
	FORCE_INLINE NO_DISCARD float32 GetHorizontalBorder() const NOEXCEPT
	{
		return _HorizontalBorder;
	}

	/*
	*	Sets the horizontal border.
	*/
	FORCE_INLINE void SetHorizontalBorder(const float32 value) NOEXCEPT
	{
		_HorizontalBorder = value;
	}

	/*
	*	Returns the motion blur intensity.
	*/
	FORCE_INLINE NO_DISCARD float32 GetMotionBlurIntensity() const NOEXCEPT
	{
		return _MotionBlurIntensity;
	}

	/*
	*	Sets the motion blur intensity.
	*/
	FORCE_INLINE void SetMotionBlurIntensity(const float32 value) NOEXCEPT
	{
		_MotionBlurIntensity = value;
	}

	/*
	*	Returns the color grading texture resource.
	*/
	FORCE_INLINE NO_DISCARD ResourcePointer<Texture2DResource> GetColorGradingTextureResource() const NOEXCEPT
	{
		return _ColorGradingTextureResource;
	}

	/*
	*	Sets the color grading texture resource.
	*/
	FORCE_INLINE void SetColorGradingTextureResource(const ResourcePointer<Texture2DResource> resource) NOEXCEPT
	{
		_ColorGradingTextureResource = resource;
	}

	/*
	*	Returns the temporal anti aliasing weight override.
	*/
	FORCE_INLINE NO_DISCARD float32 GetTemporalAntiAliasingWeightOverride() const NOEXCEPT
	{
		return _TemporalAntiAliasingWeightOverride;
	}
	
	/*
	*	Sets the temporal anti aliasing weight override.
	*/
	FORCE_INLINE void SetTemporalAntiAliasingWeightOverride(const float32 value) NOEXCEPT
	{
		_TemporalAntiAliasingWeightOverride = value;
	}

private:

	//The brightness.
	float32 _Brightness{ 1.0f };

	//The contrast.
	float32 _Contrast{ 1.0f };

	//The chromatic aberration intensity.
	float32 _ChromaticAberrationIntensity{ 0.0025f };

	//The depth of field focus distance.
	float32 _DepthOfFieldFocusDistance{ 1'024.0f };

	//The depth of field size.
	float32 _DepthOfFieldSize{ 0.001f };

	//The exposure.
	float32 _Exposure{ 1.0f };

	//The film grain intensity.
	float32 _FilmGrainIntensity{ 0.01f };

	//The horizontal border.
	float32 _HorizontalBorder{ 0.0f };

	//The motion blur intensity.
	float32 _MotionBlurIntensity{ 1.0f };

	//The color grading texture resource.
	ResourcePointer<Texture2DResource> _ColorGradingTextureResource;

	//The temporal anti aliasing weight override.
	Optional<float32> _TemporalAntiAliasingWeightOverride;

};