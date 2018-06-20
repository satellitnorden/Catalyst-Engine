#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class PostProcessingManager final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingManager);

	/*
	*	Default constructor.
	*/
	PostProcessingManager() NOEXCEPT = default;

	/*
	*	Returns whether or not any post processing value has been updated.
	*/
	bool GetHasBeenUpdated() const NOEXCEPT { return hasBeenUpdated; }

	/*
	*	Sets whether or not any post processing value has been updated.
	*/
	void SetHasBeenUpdated(const bool newHasBeenUpdated) NOEXCEPT { hasBeenUpdated = newHasBeenUpdated; }

	/*
	*	Returns the bloom strength.
	*/
	float GetBloomStrength() const NOEXCEPT { return bloomStrength; }

	/*
	*	Sets the bloom strength.
	*/
	void SetBloomStrength(const float newBloomStrength) NOEXCEPT
	{
		bloomStrength = newBloomStrength;
		hasBeenUpdated = true;
	}

	/*
	*	Returns the blur strength.
	*/
	float GetBlurStrength() const NOEXCEPT { return blurStrength; }

	/*
	*	Sets the blur strength.
	*/
	void SetBlurStrength(const float newBlurStrength) NOEXCEPT
	{
		blurStrength = newBlurStrength;
		hasBeenUpdated = true;
	}

	/*
	*	Returns the chromatic abberation strength.
	*/
	float GetChromaticAbberationStrength() const NOEXCEPT { return chromaticAberrationStrength; }

	/*
	*	Sets the chromatic abberation strength.
	*/
	void SetChromaticAbberationStrength(const float newChromaticAbberationStrength) NOEXCEPT
	{
		chromaticAberrationStrength = newChromaticAbberationStrength;
		hasBeenUpdated = true;
	}

	/*
	*	Returns the saturation strength.
	*/
	float GetSaturationStrength() const NOEXCEPT { return saturationStrength; }

	/*
	*	Sets the saturation strength.
	*/
	void SetSaturationStrength(const float newSaturationStrength) NOEXCEPT
	{
		saturationStrength = newSaturationStrength;
		hasBeenUpdated = true;
	}

	/*
	*	Returns the sharpen strength.
	*/
	float GetSharpenStrength() const NOEXCEPT { return sharpenStrength; }

	/*
	*	Sets the sharpen strength.
	*/
	void SetSharpenStrength(const float newSharpenStrength) NOEXCEPT
	{
		sharpenStrength = newSharpenStrength;
		hasBeenUpdated = true;
	}

private:

	//Denotes whether or not any post processing value has been updated.
	bool hasBeenUpdated{ true };

	//The bloom strength.
	float bloomStrength{ 0.25f };

	//The blur strength.
	float blurStrength{ 0.0f };

	//The chromatic aberration strength.
	float chromaticAberrationStrength{ 0.0f };

	//The saturation strength.
	float saturationStrength{ 1.0f };

	//The sharpen strength.
	float sharpenStrength{ 0.0f };

};