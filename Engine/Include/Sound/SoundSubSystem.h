#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class SoundSystem;

class SoundSubSystem
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
#if defined(CATALYST_PLATFORM_WINDOWS)
		WASAPI,
		ASIO,
#endif

		DEFAULT //This will pick the default (first) sub system for each platform.
	};

	/*
	*	Initialization parameters class definition.
	*/
	class InitializationParameters final
	{

	};

	//The sound system.
	SoundSystem *RESTRICT _SoundSystem;

	//The type.
	Type _Type;

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~SoundSubSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes this sound sub system.
	*/
	virtual void Initialize(const InitializationParameters &initialization_parameters) NOEXCEPT = 0;

	/*
	*	Terminates this sound sub system.
	*/
	virtual void Terminate() NOEXCEPT = 0;

	/*
	*	Returns if this sound sub system is initialized.
	*/
	virtual NO_DISCARD bool IsInitialized() const NOEXCEPT = 0;

	/*
	*	Returns the number of channels for the chosen audio output device.
	*/
	virtual NO_DISCARD uint8 GetNumberOfChannels() const NOEXCEPT = 0;

	/*
	*	Returns the sample rate for the chosen audio output device.
	*/
	virtual NO_DISCARD float32 GetSampleRate() const NOEXCEPT = 0;

	/*
	*	Returns the sound format.
	*/
	virtual NO_DISCARD SoundFormat GetSoundFormat() const NOEXCEPT = 0;

};