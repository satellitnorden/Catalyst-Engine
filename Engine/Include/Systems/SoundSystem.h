#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/SoundResource.h>

//Sound.
#include <Sound/SoundCore.h>

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

private:

	/*
	*	The asynchronous update function.
	*/
	void AsynchronousUpdate() NOEXCEPT;

	/*
	*	The sound callback.
	*/
	void SoundCallback(	const float32 sample_rate,
						const uint8 bits_per_sample,
						const uint8 number_of_channels,
						const uint32 number_of_samples,
						void *const RESTRICT buffer_data) NOEXCEPT;

};