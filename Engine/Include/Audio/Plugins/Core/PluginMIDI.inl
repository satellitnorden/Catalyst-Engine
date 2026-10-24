namespace Audio
{

	//////////
	// MIDI //
	//////////

	/*
	*	Adds a MIDI message, optionally with a list of sample offsets.
	*/
	FORCE_INLINE void Plugin::AddMIDIMessage
	(
		const MIDIMessage &message,
		const uint32 *const RESTRICT sample_offset
	) NOEXCEPT
	{
		//Add the requests.
		{
			SCOPED_LOCK(_RequestsLock);

			_Requests.Emplace();
			Request& request{ _Requests.Back() };

			request._Type = Request::Type::ADD_MIDI_MESSAGE;
			request._AddMIDIMessageData._Message = message;
			request._AddMIDIMessageData._SampleOffset = sample_offset ? (*sample_offset) : 0;
		}

		//Update the MIDI note states.
		if (message._Type == MIDIMessage::Type::NOTE_ON)
		{
			_MIDINoteStates[message._NoteOnNote]._Pressed.Store(true);
		}

		else if (message._Type == MIDIMessage::Type::NOTE_OFF)
		{
			_MIDINoteStates[message._NoteOffNote]._Pressed.Store(false);
		}
	}

	/*
	*	Retrieves the note state for the given MIDI note.
	*/
	FORCE_INLINE NO_DISCARD const Plugin::MIDINoteState &Plugin::GetMIDINoteState(const uint8 note) const NOEXCEPT
	{
		return _MIDINoteStates[note];
	}

	/*
	*	Adds a MIDI message internally.
	*/
	FORCE_INLINE void Plugin::AddMIDIMessageInternal(const MIDIMessage &message, const uint32 sample_offset) NOEXCEPT
	{
		//Add the queued MIDI message.
		_QueuedMIDIMessages.Emplace();
		QueuedMIDIMessage &queued_midi_message{ _QueuedMIDIMessages.Back() };

		queued_midi_message._Message = message;
		queued_midi_message._SampleOffset;
	}

}