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
	*	Returns the volumetric lighting mode.
	*/
	VolumetricLightingMode GetVolumetricLightingMode() const NOEXCEPT
	{
		return _VolumetricLightingMode;
	}

private:

	//The anti aliasing mode.
	AntiAliasingMode _AntiAliasingMode{ AntiAliasingMode::FastApproximateAndTemporal };

	//The diffuse irradiance mode.
	DiffuseIrradianceMode _DiffuseIrradianceMode{ DiffuseIrradianceMode::Simple };

	//The volumetric lighting mode.
	VolumetricLightingMode _VolumetricLightingMode{ VolumetricLightingMode::RayTraced };

};