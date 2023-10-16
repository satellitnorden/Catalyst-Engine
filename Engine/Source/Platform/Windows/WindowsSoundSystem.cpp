#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/SoundSystem.h>

//Concurrency.
#include <Concurrency/Spinlock.h>
#include <Concurrency/Thread.h>

//Sound.
#include <Sound/SoundResourcePlayer.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/MemorySystem.h>

//Third party.
#include <ThirdParty/RtMidi/RtMidi.h>


//Windows sound system data.
namespace WindowsSoundSystemData
{

	//The query MIDI in.
	RtMidiIn *RESTRICT _QueryMIDIIn{ nullptr };

	//The query MIDI out.
	RtMidiOut *RESTRICT _QueryMIDIOut{ nullptr };

	//The opened MIDI ins.
	DynamicArray<RtMidiIn *RESTRICT> _OpenedMIDIIns;

	//The opened MIDI outs.
	DynamicArray<RtMidiOut *RESTRICT> _OpenedMIDIOuts;

}

/*
*	Opens the given audio device.
*/
void SoundSystem::OpenAudioDevice(AudioDevice *const RESTRICT audio_device) NOEXCEPT
{
	
}

/*
*	Initializes the platform.
*/
void SoundSystem::PlatformInitialize(const CatalystProjectSoundConfiguration &configuration) NOEXCEPT
{
	
}

/*
*	Terminates the platform.
*/
void SoundSystem::PlatformTerminate() NOEXCEPT
{
	//Destroy the query Rt Midi In.
	if (WindowsSoundSystemData::_QueryMIDIIn)
	{
		WindowsSoundSystemData::_QueryMIDIIn->~RtMidiIn(); //The MemorySystem will take care of actually freeing the memory. (:
	}

	//Destroy the opened MIDI ins.
	for (RtMidiIn *const RESTRICT opened_midi_in : WindowsSoundSystemData::_OpenedMIDIIns)
	{
		opened_midi_in->closePort();
		opened_midi_in->~RtMidiIn(); //The MemorySystem will take care of actually freeing the memory. (:
	}

	//Destroy the opened MIDI outs.
	for (RtMidiOut *const RESTRICT opened_midi_out : WindowsSoundSystemData::_OpenedMIDIOuts)
	{
		opened_midi_out->closePort();
		opened_midi_out->~RtMidiOut(); //The MemorySystem will take care of actually freeing the memory. (:
	}
}

/*
*	Queries for input MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<InputMIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
{
	//Create the query MIDI in, if necessary.
	if (!WindowsSoundSystemData::_QueryMIDIIn)
	{
		WindowsSoundSystemData::_QueryMIDIIn = new (MemorySystem::Instance->TypeAllocate<RtMidiIn>()) RtMidiIn();

		ASSERT(WindowsSoundSystemData::_QueryMIDIIn, "Couldn't create MIDI in!");
	}

	//Query the port count.
	const uint32 port_count{ WindowsSoundSystemData::_QueryMIDIIn->getPortCount() };

	//Reserve the appropriate amount.
	midi_devices->Reserve(port_count);

	//Fill in the midi devices.
	for (uint32 i{ 0 }; i < port_count; ++i)
	{
		midi_devices->Emplace();
		InputMIDIDevice &midi_device{ midi_devices->Back() };

		midi_device._Handle = nullptr;
		midi_device._Index = i;

		std::string port_name{ WindowsSoundSystemData::_QueryMIDIIn->getPortName(i) };

		midi_device._Name = port_name.substr(0, port_name.find_last_of(' ')).c_str();
	}
}

/*
*	Queries for output MIDI devices.
*/
void SoundSystem::QueryMIDIDevices(DynamicArray<OutputMIDIDevice> *const RESTRICT midi_devices) NOEXCEPT
{
	//Create the query MIDI out, if necessary.
	if (!WindowsSoundSystemData::_QueryMIDIOut)
	{
		WindowsSoundSystemData::_QueryMIDIOut = new (MemorySystem::Instance->TypeAllocate<RtMidiOut>()) RtMidiOut();

		ASSERT(WindowsSoundSystemData::_QueryMIDIOut, "Couldn't create MIDI out!");
	}

	//Query the port count.
	const uint32 port_count{ WindowsSoundSystemData::_QueryMIDIOut->getPortCount() };

	//Reserve the appropriate amount.
	midi_devices->Reserve(port_count);

	//Fill in the midi devices.
	for (uint32 i{ 0 }; i < port_count; ++i)
	{
		midi_devices->Emplace();
		OutputMIDIDevice &midi_device{ midi_devices->Back() };

		midi_device._Handle = nullptr;
		midi_device._Index = i;

		std::string port_name{ WindowsSoundSystemData::_QueryMIDIOut->getPortName(i) };

		midi_device._Name = port_name.substr(0, port_name.find_last_of(' ')).c_str();
	}
}

/*
*	Opens an input MIDI device.
*/
void SoundSystem::OpenMIDIDevice(InputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Create the MIDI in.
	midi_device->_Handle = new (MemorySystem::Instance->TypeAllocate<RtMidiIn>()) RtMidiIn();

	//Open the port.
	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->openPort(midi_device->_Index);

	//Add to the opened MIDI ins.
	WindowsSoundSystemData::_OpenedMIDIIns.Emplace(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));
}

/*
*	Opens an output MIDI device.
*/
void SoundSystem::OpenMIDIDevice(OutputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Create the MIDI out.
	midi_device->_Handle = new (MemorySystem::Instance->TypeAllocate<RtMidiOut>()) RtMidiOut();

	//Open the port.
	static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle)->openPort(midi_device->_Index);

	//Add to the opened MIDI outs.
	WindowsSoundSystemData::_OpenedMIDIOuts.Emplace(static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle));
}

/*
*	Retrieves a MIDI message from the queue of the specified input MIDI device. Returns whether or not a message was retrieved from the queue.
*/
NO_DISCARD bool SoundSystem::RetrieveMIDIMessage(InputMIDIDevice *const RESTRICT midi_device, MIDIMessage *const RESTRICT midi_message) NOEXCEPT
{
	//Get the message.
	static thread_local std::vector<uint8> message;
	message.clear();

	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->getMessage(&message);

	//Was a message received?
	if (message.empty())
	{
		return false;
	}

	else
	{
		//Is this a note on?
		if (message.size() == 3 && ((message[0] & 0xf0) == 0x90) && message[2] != 0)
		{
			midi_message->_Type = MIDIMessage::Type::NOTE_ON;
			midi_message->_NoteOnNote = message[1];
			midi_message->_NoteOnVelocity = message[2];
		}

		//Is this an aftertouch?
		else if (message.size() == 3 && ((message[0] & 0xf0) == 0xA0))
		{
			midi_message->_Type = MIDIMessage::Type::AFTERTOUCH;
			midi_message->_AftertouchNote = message[1];
		}

		//The MIDI message wasn't a valid type.
		else
		{
			return false;
		}

		//Return that the retrieval succeeded.
		return true;
	}
}

/*
*	Sends a MIDI message to the specified output MIDI device.
*/
void SoundSystem::SendMIDIMessage(OutputMIDIDevice *const RESTRICT midi_device, const MIDIMessage &midi_message) NOEXCEPT
{
	//Construct the message.
	static thread_local DynamicArray<uint8> message;
	message.Clear();

	switch (midi_message._Type)
	{
		case MIDIMessage::Type::NOTE_ON:
		{
			message.Emplace(0x90 | (0x0f & midi_message._NoteOnChannel));
			message.Emplace(midi_message._NoteOnNote & 0x7f);
			message.Emplace(midi_message._NoteOnVelocity & 0x7f);

			break;
		}

		case MIDIMessage::Type::NOTE_OFF:
		{
			message.Emplace(0x80 | (0x0f & midi_message._NoteOffChannel));
			message.Emplace(midi_message._NoteOffNote & 0x7f);
			message.Emplace(0x00);

			break;
		}

		case MIDIMessage::Type::AFTERTOUCH:
		{
			message.Emplace(0xf0);
			message.Emplace(midi_message._AftertouchNote & 0x7f);
			message.Emplace(0);

			break;
		}

		case MIDIMessage::Type::CONTROL_CHANGE:
		{
			message.Emplace(0xb0);
			message.Emplace(midi_message._ControlChangeNote & 0x7f);
			message.Emplace(midi_message._ControlChangeValue & 0x7f);

			break;
		}

		case MIDIMessage::Type::PROGRAM_CHANGE:
		{
			message.Emplace(0xc0 | (0x0f & midi_message._ProgramChangeChannel));
			message.Emplace(midi_message._ProgramChangeValue & 0x7f);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle)->sendMessage(message.Data(), message.Size());
}

/*
*	Closes an input MIDI device.
*/
void SoundSystem::CloseMIDIDevice(InputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Close the port.
	static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle)->closePort();

	//Destroy the MIDI in.
	MemorySystem::Instance->TypeFree<RtMidiIn>(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));

	//Remove from the opened MIDI ins.
	WindowsSoundSystemData::_OpenedMIDIIns.Erase<false>(static_cast<RtMidiIn *const RESTRICT>(midi_device->_Handle));
}

/*
*	Closes an output MIDI device.
*/
void SoundSystem::CloseMIDIDevice(OutputMIDIDevice *const RESTRICT midi_device) NOEXCEPT
{
	//Close the port.
	static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle)->closePort();

	//Destroy the MIDI out.
	MemorySystem::Instance->TypeFree<RtMidiOut>(static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle));

	//Remove from the opened MIDI outs.
	WindowsSoundSystemData::_OpenedMIDIOuts.Erase<false>(static_cast<RtMidiOut *const RESTRICT>(midi_device->_Handle));
}
#endif