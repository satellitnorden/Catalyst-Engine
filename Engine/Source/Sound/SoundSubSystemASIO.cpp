#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Sound/SoundSubSystemASIO.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/SoundSystem.h>

/*
*	Initializes this sound sub system.
*/
void SoundSubSystemASIO::Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT
{
	
}

/*
*	Terminates this sound sub system.
*/
void SoundSubSystemASIO::Terminate() NOEXCEPT
{
	
}

/*
*	The update function.
*/
void SoundSubSystemASIO::Update() NOEXCEPT
{
	
}

/*
*	Returns the audio latency.
*	That is, the time between a sound is requested to be played until it is heard.
*	This gives an estimate, and might be a bit unreliable on certain platforms.
*	The returned value is in milliseconds.
*/
NO_DISCARD float32 SoundSubSystemASIO::GetAudioLatency() const NOEXCEPT
{
	//Should have practically no latency, no?
	return 0.0f;
}
#endif