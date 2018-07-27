#pragma once

//Core.
#include <Core/EngineCore.h>

class ClairvoyantSoundDirector final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantSoundDirector);

	/*
	*	Default constructor.
	*/
	ClairvoyantSoundDirector() NOEXCEPT { }

	/*
	*	Initializes the Clairvoyant sound director.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the Clairvoyant sound director.
	*/
	void Update() NOEXCEPT;

	/*
	*	Releases the Clairvoyant sound director.
	*/
	void Release() NOEXCEPT;

};