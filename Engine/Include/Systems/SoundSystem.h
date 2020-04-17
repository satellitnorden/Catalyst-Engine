#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/General/Vector.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/SoundResource.h>

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
	SoundSystem() NOEXCEPT
	{
	
	}

	/*
	*	Initializes the sound system.
	*/
	void Initialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT;

	/*
	*	Terminates the sound system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	uint8 GetNumberOfChannels() const NOEXCEPT;

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	uint32 GetSampleRate() const NOEXCEPT;

	/*
	*	Plays a sound.
	*/
	void PlaySound(const ResourcePointer<SoundResource> resource) NOEXCEPT;

};