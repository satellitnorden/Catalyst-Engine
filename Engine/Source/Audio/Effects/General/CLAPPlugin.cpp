//Header file.
#include <Audio/Effects/General/CLAPPlugin.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <ThirdParty/CLAP/ext/draft/resource-directory.h>

/*
*	The 'get_extension' function.
*/
const void *get_extension(const clap_host *host, const char *extension_id)
{
	if (StringUtilities::IsEqual(extension_id, CLAP_EXT_LOG))
	{
		static clap_host_log_t LOG_EXTENSION;

		LOG_EXTENSION.log = [](const clap_host_t *host, clap_log_severity severity, const char *msg)
		{
			switch (severity)
			{
				case CLAP_LOG_DEBUG:
				case CLAP_LOG_INFO:
				{
					LOG_INFORMATION(msg);

					break;
				}

				case CLAP_LOG_WARNING:
				{
					LOG_WARNING(msg);

					break;
				}

				case CLAP_LOG_ERROR:
				case CLAP_LOG_HOST_MISBEHAVING:
				case CLAP_LOG_PLUGIN_MISBEHAVING:
				{
					LOG_ERROR(msg);

					break;
				}

				case CLAP_LOG_FATAL:
				{
					LOG_FATAL(msg);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		};

		return &LOG_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_THREAD_CHECK))
	{
		static clap_host_thread_check_t THREAD_CHECK_EXTENSION;

		THREAD_CHECK_EXTENSION.is_main_thread = [](const clap_host_t *host)
		{
			return true;
		};
		THREAD_CHECK_EXTENSION.is_audio_thread = [](const clap_host_t *host)
		{
			return true;
		};

		return &THREAD_CHECK_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_THREAD_POOL))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_AUDIO_PORTS))
	{
		static clap_host_audio_ports_t AUDIO_PORTS_EXTENSION;

		AUDIO_PORTS_EXTENSION.is_rescan_flag_supported = [](const clap_host_t *host, uint32_t flag)
		{
			ASSERT(false, "This should probably be supported, no?");
			return false;
		};
		AUDIO_PORTS_EXTENSION.rescan = [](const clap_host_t *host, uint32_t flags)
		{
			//Do nothing. :x
		};

		return &AUDIO_PORTS_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_AUDIO_PORTS_CONFIG))
	{
		static clap_host_audio_ports_config AUDIO_PORTS_CONFIG_EXTENSION;

		AUDIO_PORTS_CONFIG_EXTENSION.rescan = [](const clap_host_t *host)
		{
			ASSERT(false, "Not entirely sure what to do here?");
		};

		return &AUDIO_PORTS_CONFIG_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_NOTE_PORTS))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_TIMER_SUPPORT))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_POSIX_FD_SUPPORT))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_RESOURCE_DIRECTORY))
	{
		static clap_host_resource_directory RESOURCE_DIRECTORY_EXTENSION;

		RESOURCE_DIRECTORY_EXTENSION.request_directory = [](const clap_host_t *host, bool is_shared)
		{
			ASSERT(false, "Figure out what this is supposed to do!");
			return false;
		};
		RESOURCE_DIRECTORY_EXTENSION.release_directory = [](const clap_host_t *host, bool is_shared)
		{
			ASSERT(false, "Figure out what this is supposed to do!");
		};

		return &RESOURCE_DIRECTORY_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_LATENCY))
	{
		static clap_host_latency LATENCY_EXTENSION;

		LATENCY_EXTENSION.changed = [](const clap_host_t *host)
		{
			ASSERT(false, "Figure out what this is supposed to do!");
		};

		return &LATENCY_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_GUI))
	{
		//Don't support GUI's for now. (:
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_PARAMS))
	{
		static clap_host_params PARAMS_EXTENSION;

		PARAMS_EXTENSION.rescan = [](const clap_host_t *host, clap_param_rescan_flags flags)
		{
			ASSERT(false, "Figure out what to do here!");
		};
		PARAMS_EXTENSION.clear = [](const clap_host_t *host, clap_id param_id, clap_param_clear_flags flags)
		{
			ASSERT(false, "Figure out what to do here!");
		};
		PARAMS_EXTENSION.request_flush = [](const clap_host_t *host)
		{
			ASSERT(false, "Figure out what to do here!");
		};

		return &PARAMS_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_TRACK_INFO))
	{
		static clap_host_track_info TRACK_INFO_EXTENSION;

		TRACK_INFO_EXTENSION.get = [](const clap_host_t *host, clap_track_info_t *info)
		{
			constexpr const char* const RESTRICT TRACK_NAME{ "Catalyst Engine CLAP Audio Track" };

			info->flags = CLAP_TRACK_INFO_HAS_TRACK_NAME | CLAP_TRACK_INFO_HAS_TRACK_COLOR | CLAP_TRACK_INFO_HAS_AUDIO_CHANNEL;
			Memory::Copy(info->name, TRACK_NAME, StringUtilities::StringLength(TRACK_NAME) + 1);
			info->color.red = 255;
			info->color.green = 0;
			info->color.blue = 0;
			info->color.alpha = 255;
			info->audio_channel_count = 2;
			info->audio_port_type = "CLAP_PORT_STEREO";

			return true;
		};

		return &TRACK_INFO_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_STATE))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_NOTE_NAME))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_REMOTE_CONTROLS))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_REMOTE_CONTROLS_COMPAT))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_VOICE_INFO))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_CONTEXT_MENU))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_CONTEXT_MENU_COMPAT))
	{
		//Should be fine, no?
		return nullptr;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_PRESET_LOAD))
	{
		//Should be fine, no?
		return nullptr;
	}

	else
	{
		ASSERT(false, "Unknown ID!");

		return nullptr;
	}
}

/*
*	Default constructor.
*/
CLAPPlugin::CLAPPlugin() NOEXCEPT
{
	//Reset the process struct.
	Memory::Set(&_Process, 0, sizeof(clap_process_t));
}

/*
*	Default destructor.
*/
CLAPPlugin::~CLAPPlugin() NOEXCEPT
{
	//Terminate.
	Terminate();
}

/*
*	Initializes this CLAP plugin.. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPlugin::Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT
{
	//Load the dynamic library.
	if (!_DynamicLibrary.Load(plugin_file_path))
	{
		ASSERT(false, "Could not load plugin dynamic library!");

		Terminate();

		return false;
	}

	//Retrieve the entry.
	_Entry = static_cast<const clap_plugin_entry *const RESTRICT>(_DynamicLibrary.GetSymbol("clap_entry"));

	if (!_Entry)
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Initialize the entry.
	if (!_Entry->init)
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	if (!_Entry->init(plugin_file_path))
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Set up the host.
	_Host.clap_version = CLAP_VERSION;
	_Host.host_data = this;
	_Host.name = "Catalyst Engine";
	_Host.vendor = "Daybreak Studio";
	_Host.url = "DaybreakStudio.com";
	_Host.version = "1.0";
	_Host.get_extension = get_extension;
	_Host.request_restart = [](const struct clap_host *host) -> void
	{
		ASSERT(false, "Implement this!");
	};
	_Host.request_process = [](const struct clap_host *host) -> void
	{
		ASSERT(false, "Implement this!");
	};
	_Host.request_callback = [](const struct clap_host *host) -> void
	{
		//ASSERT(false, "Implement this!");
	};

	//Retrieve the factory.
	if (!_Entry->get_factory)
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	const clap_plugin_factory *const RESTRICT factory{ static_cast<const clap_plugin_factory *const RESTRICT>(_Entry->get_factory(CLAP_PLUGIN_FACTORY_ID)) };

	if (!factory)
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Enumerate the plugins.
	const uint32 plugin_count{ factory->get_plugin_count(factory) };

	if (plugin_count != 1)
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Retrieve the plugin descriptor.
	const clap_plugin_descriptor_t *const RESTRICT plugin_descriptor{ factory->get_plugin_descriptor(factory, 0) };

	//Create the plugin!
	_Plugin = factory->create_plugin(factory, &_Host, plugin_descriptor->id);

	//Initialize the plugin.
	if (!_Plugin->init(_Plugin))
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Activate the plugin.
	if (!_Plugin->activate(_Plugin, static_cast<float64>(_SampleRate), 1, 4096))
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Start processing.
	if (!_Plugin->start_processing(_Plugin))
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Retrieve the parameters.
	const clap_plugin_params *const RESTRICT parameters{ static_cast<const clap_plugin_params *const RESTRICT>(_Plugin->get_extension(_Plugin, CLAP_EXT_PARAMS)) };

	if (parameters)
	{
		//Fill in the parameters.
		const uint32 parameter_count{ parameters->count(_Plugin) };

		_Parameters.Reserve(parameter_count);

		for (uint32 parameter_index{ 0 }; parameter_index < parameter_count; ++parameter_index)
		{
			clap_param_info_t parameter_info;
			parameters->get_info(_Plugin, parameter_index, &parameter_info);

			_Parameters.Emplace();
			Parameter &new_parameter{ _Parameters.Back() };

			new_parameter._Name = parameter_info.name;
			new_parameter._Identifier = HashString(new_parameter._Name.Data());
			new_parameter._ID = parameter_info.id;
			new_parameter._Cookie = parameter_info.cookie;
			new_parameter._MinimumValue = parameter_info.min_value;
			new_parameter._MaximumValue = parameter_info.max_value;
			new_parameter._DefaultValue = parameter_info.default_value;
		}
	}
}

/*
*	Terminates this CLAP plugin.
*/
void CLAPPlugin::Terminate() NOEXCEPT
{
	//Terminate the plugin.
	if (_Plugin)
	{
		//Tell the plugin to stop processing.
		_Plugin->stop_processing(_Plugin);

		//Deactivate the plugin.
		_Plugin->deactivate(_Plugin);

		//Destroy the plugin.
		_Plugin->destroy(_Plugin);

		//Reset the plugin.
		_Plugin = nullptr;
	}

	//Deinitialize the entry.
	if (_Entry && _Entry->deinit)
	{
		_Entry->deinit();
		_Entry = nullptr;
	}

	//Unload the dynamic library.
	_DynamicLibrary.Unload();
}

/*
*	Callback for this audio effect to process the given buffer.
*/
void CLAPPlugin::Process
(
	const AudioProcessContext &context,
	const DynamicArray<DynamicArray<float32>> &inputs,
	DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
	const uint8 number_of_channels,
	const uint32 number_of_samples
) NOEXCEPT
{
	//If we don't have a plugin, just copy inputs into outputs and be done with it.
	if (!_Plugin)
	{
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		return;
	}

	//Set up the input buffer.
	_InputBuffer.Resize<true>(number_of_channels);

	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		_InputBuffer[channel_index].Resize<false>(number_of_samples);
		Memory::Copy(_InputBuffer[channel_index].Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
	}

	//Set up the process struct.
	_Process.frames_count = number_of_samples;
	_Process.transport = nullptr; //TODO!

	float32 *RESTRICT input_data_pointers[]
	{
		_InputBuffer[0].Data(),
		_InputBuffer[1].Data()
	};

	clap_audio_buffer_t input_buffer;

	input_buffer.data32 = input_data_pointers;
	input_buffer.data64 = nullptr;
	input_buffer.channel_count = number_of_channels;
	input_buffer.latency = 0; //I think?
	input_buffer.constant_mask = 0;

	_Process.audio_inputs = &input_buffer;

	float32 *RESTRICT output_data_pointers[]
	{
		(*outputs)[0].Data(),
		(*outputs)[1].Data()
	};

	clap_audio_buffer_t output_buffer;

	output_buffer.data32 = output_data_pointers;
	output_buffer.data64 = nullptr;
	output_buffer.channel_count = number_of_channels;
	output_buffer.latency = 0; //I think?
	output_buffer.constant_mask = 0;

	_Process.audio_outputs = &output_buffer;

	_Process.audio_inputs_count = 1;
	_Process.audio_outputs_count = 1;

	clap_input_events input_events;
	
	input_events.ctx = &_InputEvents;
	input_events.size = [](const struct clap_input_events *list) -> uint32_t
	{
		const DynamicArray<Event> *const RESTRICT events{ static_cast<const DynamicArray<Event> *const RESTRICT>(list->ctx) };
		return static_cast<uint32_t>(events->Size());
	};
	input_events.get = [](const struct clap_input_events *list, uint32_t index) -> const clap_event_header_t*
	{
		const DynamicArray<Event> *const RESTRICT events{ static_cast<const DynamicArray<Event> *const RESTRICT>(list->ctx) };
		return reinterpret_cast<const clap_event_header_t *const RESTRICT>(&events->At(index));
	};

	_Process.in_events = &input_events;

	clap_output_events output_events;

	output_events.ctx = nullptr;
	output_events.try_push = [](const struct clap_output_events *list, const clap_event_header_t *event) -> bool
	{
		return false;
	};

	_Process.out_events = &output_events;

	//Process!
	const clap_process_status status{ _Plugin->process(_Plugin, &_Process) };

	//Increment the steady time.
	_Process.steady_time += static_cast<int64_t>(number_of_samples);

	//Clear the input events.
	_InputEvents.Clear();
}

/*
*	Sets a parameter with the given identifier to the given value.
*/
void CLAPPlugin::SetParameter(const HashString identifier, const float64 value) NOEXCEPT
{
	for (const Parameter &parameter : _Parameters)
	{
		if (parameter._Identifier == identifier)
		{
			_InputEvents.Emplace();
			Event &input_event{ _InputEvents.Back() };

			input_event._ParameterValue.header.size = static_cast<uint32_t>(sizeof(clap_event_param_value_t));
			input_event._ParameterValue.header.time = 0;
			input_event._ParameterValue.header.space_id = 0; //TODO: Figure out what this does?
			input_event._ParameterValue.header.type = CLAP_EVENT_PARAM_VALUE;
			input_event._ParameterValue.header.flags = 0; //TODO: Figure out if this should have any flag?

			input_event._ParameterValue.param_id = parameter._ID;
			input_event._ParameterValue.cookie = parameter._Cookie;

			input_event._ParameterValue.note_id = -1;
			input_event._ParameterValue.port_index = -1;
			input_event._ParameterValue.channel = -1;
			input_event._ParameterValue.key = -1;

			input_event._ParameterValue.value = value;

			return;
		}
	}

	ASSERT(false, "Couldn't find parameter!");
}