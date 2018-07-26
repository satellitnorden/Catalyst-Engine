#pragma once

//Core.
#include <Core/EngineCore.h>

class ClairvoyantParticleSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantParticleSystem);

	/*
	*	Default constructor.
	*/
	ClairvoyantParticleSystem() NOEXCEPT = default;

	/*
	*	Initializes the particle system.
	*/
	void InitializeSystem() NOEXCEPT;

};