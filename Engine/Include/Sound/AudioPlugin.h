#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Sound.
#include <Sound/MIDIMessage.h>

/*
*	This is a base class for audio plugins, meant to represent some generic processing unit that can be reused in different contexts.
*/
class AudioPlugin
{

public:

	/*
	*	Parameters class definition.
	*/
	class Parameters final
	{

	public:

		//The directory that this plugin operates in.
		DynamicString _Directory;

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~AudioPlugin() NOEXCEPT
	{

	}

	/*
	*	Initializes this audio plugin.
	*/
	FORCE_INLINE virtual void Initialize(const Parameters &parameters) NOEXCEPT
	{
		//Store the parameters.
		_Parameters = parameters;
	}

	/*
	*	Callback for this audio plugin to process the given buffer.
	*/
	FORCE_INLINE virtual void Process(const float32 input, float32 *const RESTRICT output, const uint8 channel_index) NOEXCEPT
	{
		//Default to passthrough.
		*output = input;
	}

	/*
	*	Advances this audio plugin.
	*/
	FORCE_INLINE virtual void Advance() NOEXCEPT
	{

	}

	/*
	*	Callback for when this plugin receives MIDI messages.
	*/
	FORCE_INLINE virtual void OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT
	{

	}

protected:

	//The parameters.
	Parameters _Parameters;

};