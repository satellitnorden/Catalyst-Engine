#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/EnvironmentMaterial.h>

class EnvironmentSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EnvironmentSystem);

	/*
	*	Returns the environment blend.
	*/
	float GetEnvironmentBlend() const NOEXCEPT { return environmentBlend; }

	/*
	*	Sets the environment blend.
	*/
	void SetEnvironmentBlend(const float newEnvironmentBlend) NOEXCEPT { environmentBlend = newEnvironmentBlend; }

	/*
	*	Returns the night environment material.
	*/
	const EnvironmentMaterial& GetNightEnvironmentMaterial() const NOEXCEPT { return nightEnvironmentMaterial; }

	/*
	*	Sets the night environment material.
	*/
	void SetNightEnvironmentMaterial(const EnvironmentMaterial& newNightEnvironmentMaterial) NOEXCEPT { nightEnvironmentMaterial = newNightEnvironmentMaterial; }

	/*
	*	Returns the day environment material.
	*/
	const EnvironmentMaterial& GetDayEnvironmentMaterial() NOEXCEPT { return dayEnvironmentMaterial; }

	/*
	*	Sets the day environment material.
	*/
	void SetDayEnvironmentMaterial(const EnvironmentMaterial& newDayEnvironmentMaterial) NOEXCEPT { dayEnvironmentMaterial = newDayEnvironmentMaterial; }

private:

	//The current environment blend.
	float environmentBlend;

	//The current night environment material.
	EnvironmentMaterial nightEnvironmentMaterial;

	//The day night environment material.
	EnvironmentMaterial dayEnvironmentMaterial;

};