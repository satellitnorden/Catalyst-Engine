#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

class SoundSystem final
{

public:

	//Declare the system.
	DECLARE_SYSTEM(SoundSystem);

	/*
	*	Default constructor.
	*/
	SoundSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~SoundSystem() NOEXCEPT;

	/*
	*	Initializes the sound system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the sound system synschronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Releases the sound system.
	*/
	void ReleaseSystem() NOEXCEPT;

};