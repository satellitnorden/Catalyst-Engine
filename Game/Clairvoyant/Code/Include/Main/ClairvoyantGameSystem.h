#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>

class ClairvoyantGameSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantGameSystem);

	/*
	*	Default constructor.
	*/
	ClairvoyantGameSystem() NOEXCEPT { }

	/*
	*	Initializes the Clairvoyant game system.
	*/
	void InitializeSystem() NOEXCEPT;

private:

	//The sun.
	DirectionalLightEntity *RESTRICT _Sun;

};