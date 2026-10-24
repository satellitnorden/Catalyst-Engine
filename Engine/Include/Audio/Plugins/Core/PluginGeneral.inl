namespace Audio
{

	/////////////
	// GENERAL //
	/////////////

	/*
	*	Default destructor.
	*/
	FORCE_INLINE Plugin::~Plugin() NOEXCEPT
	{

	}

	/*
	*	Returns the main category.
	*/
	FORCE_INLINE NO_DISCARD Plugin::MainCategory Plugin::GetMainCategory() const NOEXCEPT
	{
		return Plugin::MainCategory::NONE;
	}

	/*
	*	Returns the sub category.
	*/
	FORCE_INLINE NO_DISCARD Plugin::SubCategory Plugin::GetSubCategory() const NOEXCEPT
	{
		return Plugin::SubCategory::NONE;
	}

	/*
	*	Returns the resolution.
	*/
	FORCE_INLINE NO_DISCARD Resolution Plugin::GetResolution() const NOEXCEPT
	{
		return Resolution(0, 0);
	}

	/*
	*	Calls the 'Initialize()' function.
	*/
	FORCE_INLINE void Plugin::CallInitialize(const InitializeContext &context) NOEXCEPT
	{
		//Initialize.
		Initialize(context);

		//Set up the parameter state.
		if (!_Parameters.Empty())
		{
			_ParameterState.Reserve(_Parameters.Size());

			for (const Parameter &parameter : _Parameters)
			{
				//Cache the current.
				const float32 current{ ParameterDataToPlain(parameter._Data, parameter._Type) };

				_ParameterState.Emplace();
				ParameterState &parameter_state{ _ParameterState.Back() };

				parameter_state._Identifier = HashString(parameter._Name.Data());
				parameter_state._Default = current;
				parameter_state._Current.Store(current);
			}
		}

		//Post-initialize.
		PostInitialize(context);
	}

	/*
	*	Calls the 'Process()' function.
	*/
	FORCE_INLINE void Plugin::CallProcess
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT
	{
		//Define constants.
		constexpr auto Output
		{
			[]
			(
				Audio::Plugin *const RESTRICT _this,
				const AudioProcessContext &context,
				DynamicArray<DynamicArray<float32>> *const RESTRICT inputs,
				DynamicArray<DynamicArray<float32>> *const RESTRICT temporary_outputs,
				DynamicArray<DynamicArray<float32>> *const RESTRICT final_outputs,
				const uint8 number_of_channels,
				uint32 *const RESTRICT last_output_index,
				const uint32 sample_index
			)
			{
				//Cache the number of samples.
				const uint32 number_of_samples{ static_cast<uint32>(inputs->At(0).Size()) };

				//Process!
				_this->Process(context, (*inputs), temporary_outputs, number_of_channels, number_of_samples);

				//Copy the outputs.
				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					Memory::Copy(&final_outputs->At(channel_index).At((*last_output_index)), temporary_outputs->At(channel_index).Data(), number_of_samples * sizeof(float32));
				}

				//Update the last output index.
				(*last_output_index) = sample_index;

				//Clear the buffers.
				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					inputs->At(channel_index).Clear();
					temporary_outputs->At(channel_index).Clear();
				}
			}
		};

		//Process requests.
		ProcessRequests();

		//If there's no queued MIDI messages, just process as is.
		if (_QueuedMIDIMessages.Empty())
		{
			Process(context, inputs, outputs, number_of_channels, number_of_samples);
		}

		//Otherwise, we need to split the buffer up, and call 'OnMIDIMessages' in between.
		else
		{
			//Ensure the inputs/outputs have enough data.
			_Inputs.Resize<true>(number_of_channels);
			_Outputs.Resize<true>(number_of_channels);

			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				_Inputs.At(channel_index).Clear();
				_Outputs.At(channel_index).Clear();
			}

			//Remember the last output index.
			uint32 last_output_index{ 0 };

			//Iterate over each samples, adding to the inputs/outputs and output inbetween.
			for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
			{
				//Check if any queued MIDI messages need triggering.
				bool should_output{ false };

				for (const QueuedMIDIMessage &queued_midi_message : _QueuedMIDIMessages)
				{
					if (queued_midi_message._SampleOffset == sample_index)
					{
						OnMIDIMessage(queued_midi_message._Message);

						should_output = true;
					}
				}

				//Check if we can output.
				const bool can_output{ !_Inputs.At(0).Empty() };

				//If we should and can output, then output!
				if (should_output && can_output)
				{
					Output
					(
						this,
						context,
						&_Inputs,
						&_Outputs,
						outputs,
						number_of_channels,
						&last_output_index,
						sample_index
					);
				}

				//Add to the buffers.
				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					_Inputs.At(channel_index).Emplace(inputs.At(channel_index).At(sample_index));
					_Outputs.At(channel_index).Emplace(0.0f);
				}
			}

			//Do one last output, if necessary.
			if (!_Inputs.At(0).Empty())
			{
				Output
				(
					this,
					context,
					&_Inputs,
					&_Outputs,
					outputs,
					number_of_channels,
					&last_output_index,
					UINT32_MAXIMUM
				);
			}

			//Clear the queued MIDI messages.
			_QueuedMIDIMessages.Clear();
		}
	}

	/*
	*	Initializes this plugin.
	*/
	FORCE_INLINE void Plugin::Initialize(const InitializeContext &context) NOEXCEPT
	{

	}

	/*
	*	Post-initializes this plugin.
	*/
	FORCE_INLINE void Plugin::PostInitialize(const InitializeContext &context) NOEXCEPT
	{

	}

	/*
	*	Callback for this plugin to process the given buffer.
	*/
	FORCE_INLINE void Plugin::Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT
	{
		for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				outputs->At(channel_index).At(sample_index) = inputs.At(channel_index).At(sample_index);
			}
		}
	}

	/*
	*	Callback for this plugin when it receives a MIDI message.
	*/
	FORCE_INLINE void Plugin::OnMIDIMessage(const MIDIMessage &message) NOEXCEPT
	{

	}

	/*
	*	Processes requests.
	*/
	FORCE_INLINE void Plugin::ProcessRequests() NOEXCEPT
	{
		SCOPED_LOCK(_RequestsLock);

		for (const Request &request : _Requests)
		{
			switch (request._Type)
			{
				case Request::Type::NONE:
				{
					ASSERT(false, "Request must have a type!");

					break;
				}

				case Request::Type::SET_PARAMETER:
				{
					SetParameterInternal(request._SetParameterValueData._Index, request._SetParameterValueData._Value);

					break;
				}

				case Request::Type::ADD_MIDI_MESSAGE:
				{
					AddMIDIMessageInternal(request._AddMIDIMessageData._Message, request._AddMIDIMessageData._SampleOffset);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		_Requests.Clear();
	}

}