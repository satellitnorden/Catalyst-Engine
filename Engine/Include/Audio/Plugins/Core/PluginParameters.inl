namespace Audio
{

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
	*	Returns the parameter display name at the given index.
	*/
	FORCE_INLINE NO_DISCARD const char *const RESTRICT Plugin::GetParameterDisplayName(const uint64 index) const NOEXCEPT
	{
		return _Parameters[index]._DisplayName ? _Parameters[index]._DisplayName.Data() : _Parameters[index]._Name.Data();
	}

	/*
	*	Returns if the parameter is stepped.
	*/
	FORCE_INLINE NO_DISCARD bool Plugin::IsParameterStepped(const uint64 index) const NOEXCEPT
	{
		switch (_Parameters[index]._Type)
		{
			case Parameter::Type::NONE:
			{
				ASSERT(false, "Parameter needs a type!");

				return false;
			}

			case Parameter::Type::BOOL:
			case Parameter::Type::ENUMERATION:
			{
				return true;
			}

			case Parameter::Type::GAIN:
			case Parameter::Type::TIME:
			{
				return false;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return false;
			}
		}
	}

	/*
	*	Returns if the parameter is an enumeration.
	*/
	FORCE_INLINE NO_DISCARD bool Plugin::IsParameterEnumeration(const uint64 index) const NOEXCEPT
	{
		return _Parameters[index]._Type == Parameter::Type::ENUMERATION;
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
	*	Returns the parameter step normalized at the given index.
	*/
	FORCE_INLINE NO_DISCARD float32 Plugin::GetParameterStepNormalized(const uint64 index) const NOEXCEPT
	{
		switch (_Parameters[index]._Type)
		{
			case Parameter::Type::NONE:
			{
				ASSERT(false, "Parameter needs a type!");

				return 0.0f;
			}

			case Parameter::Type::BOOL:
			{
				return 1.0f;
			}

			case Parameter::Type::ENUMERATION:
			{
				return 1.0f / (_Parameters[index]._Maximum - _Parameters[index]._Minimum);
			}

			case Parameter::Type::GAIN:
			{
				return 0.0f;
			}

			case Parameter::Type::TIME:
			{
				return 0.0f;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return false;
			}
		}
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

			case Parameter::Type::BOOL:
			{
				sprintf_s(buffer, buffer_capacity, "%s", value >= 0.5f ? "On" : "Off");

				return true;
			}

			case Parameter::Type::ENUMERATION:
			{
				sprintf_s(buffer, buffer_capacity, "%s", _Parameters[index]._EnumerationNames[BaseMath::Round<uint32>(value)].Data());

				return true;
			}

			case Parameter::Type::GAIN:
			{
				sprintf_s(buffer, buffer_capacity, "%.1fdB", value);

				return true;
			}

			case Parameter::Type::TIME:
			{
				if (value >= 1'000.0f)
				{
					sprintf_s(buffer, buffer_capacity, "%.1fs", value / 1'000.0f);
				}
				
				else
				{
					sprintf_s(buffer, buffer_capacity, "%.0fms", value);
				}

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

			case Parameter::Type::BOOL:
			{
				*static_cast<bool *const RESTRICT>(_Parameters[index]._Data) = value >= 0.5f;

				break;
			}

			case Parameter::Type::ENUMERATION:
			{
				*static_cast<uint32 *const RESTRICT>(_Parameters[index]._Data) = BaseMath::Round<uint32>(value);

				break;
			}

			case Parameter::Type::GAIN:
			{
				*static_cast<float32 *const RESTRICT>(_Parameters[index]._Data) = Audio::DecibelsToGain(value);

				break;
			}

			case Parameter::Type::TIME:
			{
				*static_cast<float32 *const RESTRICT>(_Parameters[index]._Data) = value / 1'000.0f;

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

			case Parameter::Type::BOOL:
			{
				return static_cast<float32>(*static_cast<const bool *const RESTRICT>(data));
			}

			case Parameter::Type::ENUMERATION:
			{
				return static_cast<float32>(*static_cast<const uint32 *const RESTRICT>(data));
			}

			case Parameter::Type::GAIN:
			{
				return Audio::GainToDecibels(*static_cast<const float32 *const RESTRICT>(data));
			}

			case Parameter::Type::TIME:
			{
				return *static_cast<const float32 *const RESTRICT>(data) * 1'000.0f;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return 0.0f;
			}
		}
	}

}