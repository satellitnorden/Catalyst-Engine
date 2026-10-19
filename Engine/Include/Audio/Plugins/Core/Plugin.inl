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
		//Process requests.
		ProcessRequests();

		//TODO: Handle splitting up the buffer, accounting for parameter smoothing and possibly MIDI messages arriving mid-buffer.

		//Process.
		Process(context, inputs, outputs, number_of_channels, number_of_samples);
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

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		_Requests.Clear();
	}

	////////////////
	// PARAMETERS //
	////////////////

	/*
	*	Returns the number of parameters.
	*/
	FORCE_INLINE NO_DISCARD uint64 Plugin::GetNumberOfParameters() const NOEXCEPT
	{
		return _Parameters.Size();
	}

	/*
	*	Finds the parameter index with the given name.
	*	Returns UINT64_MAXIMUM if it failed.
	*/
	FORCE_INLINE NO_DISCARD uint64 Plugin::GetParameterIndex(const char *const RESTRICT name) const NOEXCEPT
	{
		//Calculate the identifier.
		const HashString identifier{ name };

		//Search through the parameters.
		for (uint64 parameter_index{ 0 }; parameter_index < _ParameterState.Size(); ++parameter_index)
		{
			if (_ParameterState[parameter_index]._Identifier == identifier)
			{
				return parameter_index;
			}
		}

		return UINT64_MAXIMUM;
	}

	/*
	*	Returns the parameter name at the given index.
	*/
	FORCE_INLINE NO_DISCARD const char *const RESTRICT Plugin::GetParameterName(const uint64 index) const NOEXCEPT
	{
		return _Parameters[index]._Name.Data();
	}

	/*
	*	Returns the parameter minimum at the given index.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::GetParameterMinimum(const uint64 index) const NOEXCEPT
	{
		return _Parameters[index]._Minimum;
	}

	/*
	*	Returns the parameter maximum at the given index.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::GetParameterMaximum(const uint64 index) const NOEXCEPT
	{
		return _Parameters[index]._Maximum;
	}

	/*
	*	Returns the parameter default at the given index.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::GetParameterDefault(const uint64 index) const NOEXCEPT
	{
		return _ParameterState[index]._Default;
	}

	/*
	*	Returns the parameter current plain at the given index.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::GetParameterCurrentPlain(const uint64 index) const NOEXCEPT
	{
		return _ParameterState[index]._Current.Load();
	}

	/*
	*	Returns the parameter current normalized at the given index.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::GetParameterCurrentNormalized(const uint64 index) const NOEXCEPT
	{
		const float32 current{ GetParameterCurrentPlain(index) };
		const float32 minimum{ GetParameterMinimum(index) };
		const float32 maximum{ GetParameterMaximum(index) };

		return (current - minimum) / (maximum - minimum);
	}

	/*
	*	Sets a parameter plain.
	*/
	FORCE_INLINE void Plugin::SetParameterPlain(const uint64 index, const float32 value) NOEXCEPT
	{
		//Set the parameter state immediately.
		_ParameterState[index]._Current.Store(value);

		//Then add the request.
		{
			SCOPED_LOCK(_RequestsLock);

			_Requests.Emplace();
			Request &request{ _Requests.Back() };

			request._Type = Request::Type::SET_PARAMETER;
			request._SetParameterValueData._Index = index;
			request._SetParameterValueData._Value = value;
		}
	}

	/*
	*	Sets a parameter normalized.
	*/
	FORCE_INLINE void Plugin::SetParameterNormalized(const uint64 index, const float32 value) NOEXCEPT
	{
		//Calculate the value.
		const float32 _value
		{
			BaseMath::LinearlyInterpolate
			(
				GetParameterMinimum(index),
				GetParameterMaximum(index),
				BaseMath::Clamp<float32>(value, 0.0f, 1.0f)
			)
		};

		//Set the parameter state immediately.
		_ParameterState[index]._Current.Store(_value);

		//Then add the request.
		{
			SCOPED_LOCK(_RequestsLock);

			_Requests.Emplace();
			Request &request{ _Requests.Back() };

			request._Type = Request::Type::SET_PARAMETER;
			request._SetParameterValueData._Index = index;
			request._SetParameterValueData._Value = _value;
		}
	}

	/*
	*	Prints the value string for the parameter with the given index.
	*/
	FORCE_INLINE NO_DISCARD bool Plugin::PrintParameterValueString(const uint64 index, char *buffer, const uint64 buffer_capacity, const float32 value) const NOEXCEPT
	{
		switch (_Parameters[index]._Type)
		{
			case Parameter::Type::NONE:
			{
				ASSERT(false, "Parameter needs a type!");

				return false;
			}

			case Parameter::Type::GAIN:
			{
				sprintf_s(buffer, buffer_capacity, "%.1fB", value);

				return true;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return false;
			}
		}
	}

	/*
	*	Sets a parameter internally.
	*/
	FORCE_INLINE void Plugin::SetParameterInternal(const uint64 index, const float32 value) NOEXCEPT
	{
		switch (_Parameters[index]._Type)
		{
			case Parameter::Type::NONE:
			{
				ASSERT(false, "Parameter needs a type!");

				break;
			}

			case Parameter::Type::GAIN:
			{
				*static_cast<float32 *const RESTRICT>(_Parameters[index]._Data) = Audio::DecibelsToGain(value);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Converts parameter data to a plain value.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::ParameterDataToPlain(const void* const RESTRICT data, const Parameter::Type type)
	{
		switch (type)
		{
			case Parameter::Type::NONE:
			{
				ASSERT(false, "Parameter needs a type!");

				return 0.0f;
			}

			case Parameter::Type::GAIN:
			{
				return Audio::GainToDecibels(*static_cast<const float32 *const RESTRICT>(data));
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0.0f;
			}
		}
	}

}