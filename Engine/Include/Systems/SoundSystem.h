#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/General/UpdateContext.h>
#include <Core/Pointers/UniquePointer.h>

//Sound.
#include <Sound/Native/SoundCore.h>

class SoundSystem final
{

public:

	//Declare the singleton.
	DECLARE_SINGLETON(SoundSystem);

	/*
	*	Default constructor.
	*/
	SoundSystem() NOEXCEPT { }

	/*
	*	Initializes the sound system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the sound system.
	*/
	void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Terminates the sound system.
	*/
	void Terminate() NOEXCEPT;

};