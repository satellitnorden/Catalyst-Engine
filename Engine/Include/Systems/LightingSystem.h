#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Pointers/UniquePointer.h>

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
	
	//The spot lights.
	StaticArray<SpotLight, LightingConstants::MAXIMUM_NUMBER_OF_POINT_LIGHTS> _SpotLights;

};