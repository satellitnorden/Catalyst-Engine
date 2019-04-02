#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Pointers/UniquePointer.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

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
	const TextureCubeHandle& GetNightEnvironmentMaterial() const NOEXCEPT { return _NightEnvironmentMaterial; }

	/*
	*	Sets the night environment material.
	*/
	void SetNightEnvironmentMaterial(const TextureCubeHandle& newNightEnvironmentMaterial) NOEXCEPT { _NightEnvironmentMaterial = newNightEnvironmentMaterial; }

	/*
	*	Returns the day environment material.
	*/
	const TextureCubeHandle& GetDayEnvironmentMaterial() NOEXCEPT { return _DayEnvironmentMaterial; }

	/*
	*	Sets the day environment material.
	*/
	void SetDayEnvironmentMaterial(const TextureCubeHandle& newDayEnvironmentMaterial) NOEXCEPT { _DayEnvironmentMaterial = newDayEnvironmentMaterial; }

private:

	//The environment blend.
	float _EnvironmentBlend;

	//The night environment material.
	TextureCubeHandle _NightEnvironmentMaterial;

	//The day environment material.
	TextureCubeHandle _DayEnvironmentMaterial;

};