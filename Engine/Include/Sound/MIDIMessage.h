#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class MIDIMessage final
{

public:

	//Enumeration covering all types.
	enum class Type : uint8
	{
		NOTE_ON,
		NOTE_OFF,
		AFTERTOUCH,
		CONTROL_CHANGE,
		PROGRAM_CHANGE
	};

	//The type.
	Type _Type;

	union
	{
		//NOTE_ON data.
		struct
		{
			uint8 _NoteOnChannel;
			uint8 _NoteOnNote;
			uint8 _NoteOnVelocity;
		};

		//NOTE_OFF data.
		struct
		{
			uint8 _NoteOffChannel;
			uint8 _NoteOffNote;
		};

		//AFTERTOUCH data.
		struct
		{
			uint8 _AftertouchNote;
		};

		//CONTROL_CHANGE data.
		struct
		{
			uint8 _ControlChangeChannel;
			uint8 _ControlChangeNote;
			uint8 _ControlChangeValue;
		};

		//PROGRAM_CHANGE data.
		struct
		{
			uint8 _ProgramChangeChannel;
			uint8 _ProgramChangeValue;
		};
	};

};