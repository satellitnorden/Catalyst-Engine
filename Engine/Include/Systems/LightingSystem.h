#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Lighting.
#include <Lighting/DirectionalLight.h>
#include <Lighting/LightingCore.h>
#include <Lighting/SpotLight.h>

class LightingSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LightingSystem);

	/*
	*	Default constructor.
	*/
	LightingSystem() NOEXCEPT { }

	/*
	*	Returns the directional light.
	*/
	RESTRICTED DirectionalLight *const RESTRICT GetDirectionalLight() NOEXCEPT
	{
		return &_DirectionalLight;
	}

private:

	//The directional light.
	DirectionalLight _DirectionalLight;

};