#if defined(CATALYST_PLATFORM_ANDROID)
//Header file.
#include <Systems/SoundSystem.h>

//Android.
#include <AAudio.h>

/*
*	Queries for audio devices.
*/
void SoundSystem::QueryAudioDevices(DynamicArray<AudioDevice> *const RESTRICT audio_devices) NOEXCEPT
{
	//Do nothing.
}

/*
*	Opens the given audio device.
*/
void SoundSystem::OpenAudioDevice(AudioDevice *const RESTRICT audio_device) NOEXCEPT
{
	//Do nothing.
}

/*
*	Initializes the platform.
*/
void SoundSystem::PlatformInitialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	//Do nothing.
}

/*
*	Returns if the platform is initialized.
*/
NO_DISCARD bool SoundSystem::PlatformInitialized() const NOEXCEPT
{
	return false;
}

/*
*	Terminates the platform.
*/
void SoundSystem::PlatformTerminate() NOEXCEPT
{
	//Do nothing.
}

/*
*	Returns the number of channels for the chosen audio output device.
*/
uint8 SoundSystem::GetNumberOfChannels() const NOEXCEPT
{
	return 0;
}

/*
*	Returns the sample rate for the chosen audio output device.
*/
float32 SoundSystem::GetSampleRate() const NOEXCEPT
{
	return 0.0f;
}

/*
*	Returns the number of bits per sample.
*/
uint8 SoundSystem::GetNumberOfBitsPerSample() const NOEXCEPT
{
	return 0;
}

/*
*	Queries for MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<MIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
{
	//Do nothing.
}

/*
*	Opens a MIDI device.
*/
void SoundSystem::OpenMIDIDevice(MIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Do nothing.
}

/*
*	Retrieves a MIDI message from the queue of the specified MIDI device. Returns whether or not a message was retrieved from the queue.
*/
NO_DISCARD bool SoundSystem::RetrieveMIDIMessage(MIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message) NOEXCEPT
{
	//Do nothing.
}

/*
*	The default asynchronous update function.
*/
void SoundSystem::DefaultAsynchronousUpdate() NOEXCEPT
{
	//Do nothing.
}
#endif