#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/EnvironmentMaterial.h>
#include <Rendering/Engine Layer/OceanMaterial.h>

class EnvironmentSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(EnvironmentSystem);

	/*
	*	Default constructor.
	*/
	EnvironmentSystem() NOEXCEPT = default;

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

	/*
	*	Returns the ocean material.
	*/
	const OceanMaterial& GetOceanMaterial() NOEXCEPT { return oceanMaterial; }

	/*
	*	Sets the ocean material.
	*/
	void SetOceanMaterial(const OceanMaterial &newOceanMaterial) NOEXCEPT { oceanMaterial = newOceanMaterial; }

private:

	//The current environment blend.
	float environmentBlend;

	//The current night environment material.
	EnvironmentMaterial nightEnvironmentMaterial;

	//The day night environment material.
	EnvironmentMaterial dayEnvironmentMaterial;

	//The ocean material.
	OceanMaterial oceanMaterial;

};