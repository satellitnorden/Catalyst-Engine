#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class ClairvoyantSoundDirector final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ClairvoyantSoundDirector);

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