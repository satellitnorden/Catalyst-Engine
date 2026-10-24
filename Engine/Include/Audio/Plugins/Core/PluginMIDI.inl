namespace Audio
{

	//////////
	// MIDI //
	//////////

	/*
	*	Adds MIDI messages, optionally with a list of sample offsets.
	*/
	FORCE_INLINE void Plugin::AddMIDIMessages
	(
		const MIDIMessage *const RESTRICT messages,
		const uint64 number_of_messages,
		const uint32 *const RESTRICT sample_offsets
	) NOEXCEPT
	{
		//Add the requests.
		SCOPED_LOCK(_RequestsLock);

		for (uint64 message_index{ 0 }; message_index < number_of_messages; ++message_index)
		{
			_Requests.Emplace();
			Request &request{ _Requests.Back() };

			request._Type = Request::Type::ADD_MIDI_MESSAGE;
			request._AddMIDIMessageData._Message = messages[message_index];
			request._AddMIDIMessageData._SampleOffset = sample_offsets ? sample_offsets[message_index] : 0;
		}
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