#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#if !defined(CATALYST_DISABLE_OCEAN)
	#include <Rendering/Engine/OceanMaterial.h>
#endif

class EnvironmentManager final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EnvironmentManager);

	/*
	*	Default constructor.
	*/
	EnvironmentManager() NOEXCEPT { }

	/*
	*	Returns the environment blend.
	*/
	float GetEnvironmentBlend() const NOEXCEPT { return _EnvironmentBlend; }

	/*
	*	Sets the environment blend.
	*/
	void SetEnvironmentBlend(const float newEnvironmentBlend) NOEXCEPT { _EnvironmentBlend = newEnvironmentBlend; }

	/*
	*	Returns the night environment material.
	*/
	const EnvironmentMaterial& GetNightEnvironmentMaterial() const NOEXCEPT { return _NightEnvironmentMaterial; }

	/*
	*	Sets the night environment material.
	*/
	void SetNightEnvironmentMaterial(const EnvironmentMaterial& newNightEnvironmentMaterial) NOEXCEPT { _NightEnvironmentMaterial = newNightEnvironmentMaterial; }

	/*
	*	Returns the day environment material.
	*/
	const EnvironmentMaterial& GetDayEnvironmentMaterial() NOEXCEPT { return _DayEnvironmentMaterial; }

	/*
	*	Sets the day environment material.
	*/
	void SetDayEnvironmentMaterial(const EnvironmentMaterial& newDayEnvironmentMaterial) NOEXCEPT { _DayEnvironmentMaterial = newDayEnvironmentMaterial; }

#if !defined(CATALYST_DISABLE_OCEAN)
	/*
	*	Returns the ocean material.
	*/
	const OceanMaterial& GetOceanMaterial() NOEXCEPT { return _OceanMaterial; }

	/*
	*	Sets the ocean material.
	*/
	void SetOceanMaterial(const OceanMaterial &newOceanMaterial) NOEXCEPT { _OceanMaterial = newOceanMaterial; }
#endif

private:

	//The environment blend.
	float _EnvironmentBlend;

	//The night environment material.
	EnvironmentMaterial _NightEnvironmentMaterial;

	//The day environment material.
	EnvironmentMaterial _DayEnvironmentMaterial;

#if !defined(CATALYST_DISABLE_OCEAN)
	//The ocean material.
	OceanMaterial _OceanMaterial;
#endif

};