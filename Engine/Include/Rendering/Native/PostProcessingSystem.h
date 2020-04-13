#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class PostProcessingSystem final
{

public:

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

private:

	//The contrast.
	float32 _Contrast{ 1.0f };

	//The chromatic aberration intensity.
	float32 _ChromaticAberrationIntensity{ 0.0025f };

	//The depth of field focus distance.
	float32 _DepthOfFieldFocusDistance{ 1'024.0f };

	//The depth of field size.
	float32 _DepthOfFieldSize{ 0.001f };

	//The film grain intensity.
	float32 _FilmGrainIntensity{ 0.01f };

};