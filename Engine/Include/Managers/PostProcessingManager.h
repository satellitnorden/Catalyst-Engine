#pragma once

//Core.
#include <Core/EngineCore.h>

class PostProcessingManager final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingManager);

	/*
	*	Default constructor.
	*/
	PostProcessingManager() NOEXCEPT { }

	/*
	*	Returns the bloom strength.
	*/
	float GetBloomStrength() const NOEXCEPT { return bloomStrength; }

	/*
	*	Sets the bloom strength.
	*/
	void SetBloomStrength(const float newBloomStrength) NOEXCEPT { bloomStrength = newBloomStrength; }

	/*
	*	Returns the blur strength.
	*/
	float GetBlurStrength() const NOEXCEPT { return blurStrength; }

	/*
	*	Sets the blur strength.
	*/
	void SetBlurStrength(const float newBlurStrength) NOEXCEPT { blurStrength = newBlurStrength; }

	/*
	*	Returns the saturation strength.
	*/
	float GetSaturationStrength() const NOEXCEPT { return saturationStrength; }

	/*
	*	Sets the saturation strength.
	*/
	void SetSaturationStrength(const float newSaturationStrength) NOEXCEPT { saturationStrength = newSaturationStrength; }

private:

	//The bloom strength.
	float bloomStrength{ 0.0f };

	//The blur strength.
	float blurStrength{ 0.0f };

	//The saturation strength.
	float saturationStrength{ 1.0f };

};