//Header file.
#include <Systems/MIDISystem.h>

//It's fine to user the RtMidi namespace directly here in this file. (:
using namespace rt::midi;

//MIDI system constants.
namespace MIDISystemConstants
{
	//The default API.
#if defined(CATALYST_PLATFORM_WINDOWS)
	constexpr RtMidi::Api DEFAULT_API{ RtMidi::Api::WINDOWS_MM };
#else
	#error "Unknown platform!";
#endif
}

/*
*	Queries devices.
*/
template <typename TYPE>
FORCE_INLINE void QueryDevices(TYPE *RESTRICT &query, DynamicArray<MIDIDeviceInformation> *const RESTRICT devices) NOEXCEPT
{
	if (!query)
	{
		query = new TYPE(MIDISystemConstants::DEFAULT_API);
	}

	devices->Clear();

	const uint32 port_count{ query->getPortCount() };
	devices->Reserve(port_count);

	for (uint32 port_index{ 0 }; port_index < port_count; ++port_index)
	{
		const std::string port_name{ query->getPortName(port_index) };

		devices->Emplace();
		MIDIDeviceInformation &device{ devices->Back() };

		device._Name = port_name.substr(0, port_name.find_last_of(' ')).c_str();
		device._Index = port_index;
	}
}

/*
*	Terminates the MIDI system.
*/
void MIDISystem::Terminate() NOEXCEPT
{
	//Destroy the queries.
	if (_InputQuery)
	{
		delete _InputQuery;
	}

	if (_OutputQuery)
	{
		delete _OutputQuery;
	}

	//Close opened devices.
	while (!_OpenedInputDevices.Empty()) CloseInputDevice(_OpenedInputDevices[0]._Identifier);
	while (!_OpenedOutputDevices.Empty()) CloseOutputDevice(_OpenedOutputDevices[0]._Identifier);
}

/*
*	Queries input devices.
*/
void MIDISystem::QueryInputDevices(DynamicArray<MIDIDeviceInformation> *const RESTRICT devices) NOEXCEPT
{
	QueryDevices(_InputQuery, devices);
}

/*
*	Queries output devices.
*/
void MIDISystem::QueryOutputDevices(DynamicArray<MIDIDeviceInformation> *const RESTRICT devices) NOEXCEPT
{
	QueryDevices(_OutputQuery, devices);
}

/*
*	Opens an input device. Returns a unique identifier for that device.
*/
NO_DISCARD Audio::Identifier MIDISystem::OpenInputDevice(const MIDIDeviceInformation &device_information) NOEXCEPT
{
	_OpenedInputDevices.Emplace();
	OpenedDevice &opened_device{ _OpenedInputDevices.Back() };

	opened_device._Identifier = _OpenedDeviceIdentifierGenerator++;
	opened_device._Handle = new RtMidiIn(MIDISystemConstants::DEFAULT_API);

	static_cast<RtMidiIn *const RESTRICT>(opened_device._Handle)->openPort(device_information._Index);

	return opened_device._Identifier;
}

/*
*	Opens an output device. Returns a unique identifier for that device.
*/
NO_DISCARD Audio::Identifier MIDISystem::OpenOutputDevice(const MIDIDeviceInformation &device_information) NOEXCEPT
{
	_OpenedOutputDevices.Emplace();
	OpenedDevice &opened_device{ _OpenedOutputDevices.Back() };

	opened_device._Identifier = _OpenedDeviceIdentifierGenerator++;
	opened_device._Handle = new RtMidiOut(MIDISystemConstants::DEFAULT_API);

	static_cast<RtMidiOut *const RESTRICT>(opened_device._Handle)->openPort(device_information._Index);

	return opened_device._Identifier;
}

/*
*	Retrieves a message from an input device with the given identifier. Returns if a message was retrieved.
*/
NO_DISCARD bool MIDISystem::RetrieveMessage(const Audio::Identifier identifier, MIDIMessage *const RESTRICT message) NOEXCEPT
{
	for (OpenedDevice &opened_device : _OpenedInputDevices)
	{
		if (opened_device._Identifier == identifier)
		{
			static thread_local std::vector<uint8> _message;
			_message.clear();

			static_cast<RtMidiIn *const RESTRICT>(opened_device._Handle)->getMessage(&_message);

			if (_message.empty())
			{
				return false;
			}

			//Is this a note on?
			if (_message.size() == 3 && ((_message[0] & 0xf0) == 0x90) && _message[2] != 0)
			{
				message->_Type = MIDIMessage::Type::NOTE_ON;
				message->_NoteOnNote = _message[1];
				message->_NoteOnVelocity = _message[2];
			}

			//Is this a note off?
			else if (_message.size() == 3 && ((_message[0] & 0xf0) == 0x80))
			{
				message->_Type = MIDIMessage::Type::NOTE_OFF;
				message->_NoteOffNote = _message[1];
			}

			//Is this an aftertouch?
			else if (_message.size() == 3 && ((_message[0] & 0xf0) == 0xA0))
			{
				message->_Type = MIDIMessage::Type::AFTERTOUCH;
				message->_AftertouchNote = _message[1];
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

	return false;
}

/*
*	Sends a message to an output device with the given identifier.
*/
void MIDISystem::SendMessage(const Audio::Identifier identifier, const MIDIMessage &message) NOEXCEPT
{
	for (OpenedDevice &opened_device : _OpenedOutputDevices)
	{
		if (opened_device._Identifier == identifier)
		{
			//Construct the message.
			static thread_local DynamicArray<uint8> _message;
			_message.Clear();

			switch (message._Type)
			{
				case MIDIMessage::Type::NOTE_ON:
				{
					_message.Emplace(0x90 | (0x0f & message._NoteOnChannel));
					_message.Emplace(message._NoteOnNote & 0x7f);
					_message.Emplace(message._NoteOnVelocity & 0x7f);

					break;
				}

				case MIDIMessage::Type::NOTE_OFF:
				{
					_message.Emplace(0x80 | (0x0f & message._NoteOffChannel));
					_message.Emplace(message._NoteOffNote & 0x7f);
					_message.Emplace(0x00);

					break;
				}

				case MIDIMessage::Type::AFTERTOUCH:
				{
					_message.Emplace(0xf0);
					_message.Emplace(message._AftertouchNote & 0x7f);
					_message.Emplace(0);

					break;
				}

				case MIDIMessage::Type::CONTROL_CHANGE:
				{
					_message.Emplace(0xb0 | (0x0f & message._ControlChangeChannel));
					_message.Emplace(message._ControlChangeNote & 0x7f);
					_message.Emplace(message._ControlChangeValue & 0x7f);

					break;
				}

				case MIDIMessage::Type::PROGRAM_CHANGE:
				{
					_message.Emplace(0xc0 | (0x0f & message._ProgramChangeChannel));
					_message.Emplace(message._ProgramChangeValue & 0x7f);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			static_cast<RtMidiOut *const RESTRICT>(opened_device._Handle)->sendMessage(_message.Data(), _message.Size());

			return;
		}
	}
}

/*
*	Closes the input device with the given identifier.
*/
void MIDISystem::CloseInputDevice(const Audio::Identifier identifier) NOEXCEPT
{
	for (uint64 i{ 0 }; i < _OpenedInputDevices.Size(); ++i)
	{
		if (_OpenedInputDevices[i]._Identifier == identifier)
		{
			static_cast<RtMidiIn *const RESTRICT>(_OpenedInputDevices[i]._Handle)->closePort();
			delete static_cast<RtMidiIn *const RESTRICT>(_OpenedInputDevices[i]._Handle);
			_OpenedInputDevices.EraseAt<false>(i);

			return;
		}
	}
}

/*
*	Closes the output device with the given identifier.
*/
void MIDISystem::CloseOutputDevice(const Audio::Identifier identifier) NOEXCEPT
{
	for (uint64 i{ 0 }; i < _OpenedOutputDevices.Size(); ++i)
	{
		if (_OpenedOutputDevices[i]._Identifier == identifier)
		{
			static_cast<RtMidiOut *const RESTRICT>(_OpenedOutputDevices[i]._Handle)->closePort();
			delete static_cast<RtMidiOut *const RESTRICT>(_OpenedOutputDevices[i]._Handle);
			_OpenedOutputDevices.EraseAt<false>(i);

			return;
		}
	}
}