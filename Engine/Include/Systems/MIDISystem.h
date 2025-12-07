#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/MIDI/MIDIDeviceInformation.h>
#include <Audio/MIDI/MIDIMessage.h>

//Systems.
#include <Systems/System.h>

//Third party.
#define RTMIDI_USE_NAMESPACE
#include <ThirdParty/RtMidi/RtMidi.h>

/*
*	System for interfacing with MIDI devices.
*/
class MIDISystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		MIDISystem,
		SYSTEM_TERMINATE()
	);

	/*
	*	Default constructor.
	*/
	MIDISystem() NOEXCEPT
	{

	}

	/*
	*	Queries input devices.
	*/
	void QueryInputDevices(DynamicArray<MIDIDeviceInformation> *const RESTRICT devices) NOEXCEPT;

	/*
	*	Queries output devices.
	*/
	void QueryOutputDevices(DynamicArray<MIDIDeviceInformation> *const RESTRICT devices) NOEXCEPT;

	/*
	*	Opens an input device. Returns a unique identifier for that device.
	*/
	NO_DISCARD Audio::Identifier OpenInputDevice(const MIDIDeviceInformation &device_information) NOEXCEPT;

	/*
	*	Opens an output device. Returns a unique identifier for that device.
	*/
	NO_DISCARD Audio::Identifier OpenOutputDevice(const MIDIDeviceInformation &device_information) NOEXCEPT;

	/*
	*	Retrieves a message from an input device with the given identifier. Returns if a message was retrieved.
	*/
	NO_DISCARD bool RetrieveMessage(const Audio::Identifier identifier, MIDIMessage *const RESTRICT message) NOEXCEPT;

	/*
	*	Sends a message to an output device with the given identifier.
	*/
	void SendMessage(const Audio::Identifier identifier, const MIDIMessage &message) NOEXCEPT;

	/*
	*	Closes the input device with the given identifier.
	*/
	void CloseInputDevice(const Audio::Identifier identifier) NOEXCEPT;

	/*
	*	Closes the output device with the given identifier.
	*/
	void CloseOutputDevice(const Audio::Identifier identifier) NOEXCEPT;

private:

	/*
	*	Opened device class definition.
	*/
	class OpenedDevice final
	{

	public:

		//The identifier.
		Audio::Identifier _Identifier;

		//The handle.
		void *RESTRICT _Handle;

	};

	//The input query.
	rt::midi::RtMidiIn *RESTRICT _InputQuery{ nullptr };

	//The output query.
	rt::midi::RtMidiOut *RESTRICT _OutputQuery{ nullptr };

	//The opened device identifier generator.
	Audio::Identifier _OpenedDeviceIdentifierGenerator{ 0 };

	//The opened input devices.
	DynamicArray<OpenedDevice> _OpenedInputDevices;

	//The opened output devices.
	DynamicArray<OpenedDevice> _OpenedOutputDevices;

};