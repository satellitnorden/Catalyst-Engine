//Header file.
#include <Audio/Effects/General/CLAPPlugin.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <ThirdParty/CLAP/ext/draft/resource-directory.h>

#define ENABLE_GUI (0)

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
			return Concurrency::CurrentThread::IsMainThread();
		};
		THREAD_CHECK_EXTENSION.is_audio_thread = [](const clap_host_t *host)
		{
			//It is assumed that any thread this plugin runs on that is not the main thread is an audio thread.
			return !Concurrency::CurrentThread::IsMainThread();
		};

		return &THREAD_CHECK_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_THREAD_POOL))
	{
		static clap_host_thread_pool_t THREAD_POOL_EXTENSION;
		static bool THREAD_POOL_EXTENSION_INITIALIZED{ false };

		if (!THREAD_POOL_EXTENSION_INITIALIZED)
		{
			THREAD_POOL_EXTENSION.request_exec = [](const clap_host_t* host, uint32_t num_tasks) -> bool
			{
				ASSERT(false, "Figure out what to do here!");

				return false;
			};

			THREAD_POOL_EXTENSION_INITIALIZED = true;
		}

		return &THREAD_POOL_EXTENSION;
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
			ASSERT(false, "Figure out what to do here!");
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
		static clap_host_note_ports_t NOTE_PORTS_EXTENSION;
		static bool NOTE_PORTS_EXTENSION_INITIALIZED{ false };

		if (!NOTE_PORTS_EXTENSION_INITIALIZED)
		{
			NOTE_PORTS_EXTENSION.supported_dialects = [](const clap_host_t *host) -> uint32_t
			{
				ASSERT(false, "Figure out what to do here!");
				return 0;
			};
			NOTE_PORTS_EXTENSION.rescan = [](const clap_host_t* host, uint32_t flags) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};

			NOTE_PORTS_EXTENSION_INITIALIZED = true;
		}

		return &NOTE_PORTS_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_TIMER_SUPPORT))
	{
		static clap_host_timer_support_t TIMER_SUPPORT_EXTENSION;
		static bool TIMER_SUPPORT_EXTENSION_INITIALIZED{ false };

		if (!TIMER_SUPPORT_EXTENSION_INITIALIZED)
		{
			TIMER_SUPPORT_EXTENSION.register_timer = [](const clap_host_t *host, uint32_t period_ms, clap_id *timer_id) -> bool
			{
				/*
				CLAPPlugin *const RESTRICT _host{ static_cast<CLAPPlugin *const RESTRICT>(host->host_data) };

				CLAPPlugin::MainThreadRequest request;

				request._Type = CLAPPlugin::MainThreadRequest::Type::ADD_TIMER;
				request._AddTimerData._Identifier = *timer_id;

				_host->AddMainThreadRequest(request);

				return true;
				*/
				ASSERT(false, "Figure out what to do here!");
				return false;
			};
			TIMER_SUPPORT_EXTENSION.unregister_timer = [](const clap_host_t *host, clap_id timer_id) -> bool
			{
				/*
				CLAPPlugin *const RESTRICT _host{ static_cast<CLAPPlugin *const RESTRICT>(host->host_data) };

				CLAPPlugin::MainThreadRequest request;

				request._Type = CLAPPlugin::MainThreadRequest::Type::REMOVE_TIMER;
				request._RemoveTimerData._Identifier = timer_id;

				_host->AddMainThreadRequest(request);

				return true;
				*/
				ASSERT(false, "Figure out what to do here!");
				return false;
			};

			TIMER_SUPPORT_EXTENSION_INITIALIZED = true;
		}

		return &TIMER_SUPPORT_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_POSIX_FD_SUPPORT))
	{
		static clap_host_posix_fd_support_t POSIX_FD_SUPPORT_EXTENSION;
		static bool POSIX_FD_SUPPORT_EXTENSION_INITIALIZED{ false };

		if (!POSIX_FD_SUPPORT_EXTENSION_INITIALIZED)
		{
			POSIX_FD_SUPPORT_EXTENSION.register_fd = [](const clap_host_t *host, int fd, clap_posix_fd_flags_t flags) -> bool
			{

				ASSERT(false, "Figure out what to do here!");
				return false;
			};
			POSIX_FD_SUPPORT_EXTENSION.modify_fd = [](const clap_host_t *host, int fd, clap_posix_fd_flags_t flags) -> bool
			{

				ASSERT(false, "Figure out what to do here!");
				return false;
			};
			POSIX_FD_SUPPORT_EXTENSION.unregister_fd = [](const clap_host_t *host, int fd) -> bool
			{

				ASSERT(false, "Figure out what to do here!");
				return false;
			};

			POSIX_FD_SUPPORT_EXTENSION_INITIALIZED = true;
		}

		return &POSIX_FD_SUPPORT_EXTENSION;
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
#if ENABLE_GUI
		static clap_host_gui GUI_EXTENSION;

		GUI_EXTENSION.resize_hints_changed = [](const clap_host_t *host) -> void
		{
			//Do nothing. (:
		};
		GUI_EXTENSION.request_resize = [](const clap_host_t *host, uint32_t width, uint32_t height) -> bool
		{
			ASSERT(false, "This should not be called if the window is floating, right?");
			return false;
		};
		GUI_EXTENSION.request_show = [](const clap_host_t *host) -> bool
		{
			CLAPPlugin *const RESTRICT _host{ static_cast<CLAPPlugin *const RESTRICT>(host->host_data) };

			CLAPPlugin::MainThreadRequest request;

			request._Type = CLAPPlugin::MainThreadRequest::Type::SHOW_GUI;

			_host->AddMainThreadRequest(request);

			return true;
		};
		GUI_EXTENSION.request_hide = [](const clap_host_t* host) -> bool
		{
			CLAPPlugin *const RESTRICT _host{ static_cast<CLAPPlugin *const RESTRICT>(host->host_data) };

			CLAPPlugin::MainThreadRequest request;

			request._Type = CLAPPlugin::MainThreadRequest::Type::HIDE_GUI;

			_host->AddMainThreadRequest(request);

			return true;
		};
		GUI_EXTENSION.closed = [](const clap_host_t *host, bool was_destroyed) -> void
		{
			//Do nothing. (:
		};

		return &GUI_EXTENSION;
#else
		//Don't support GUI's for now. (:
		return nullptr;
#endif
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
			constexpr const char *const RESTRICT TRACK_NAME{ "Catalyst Engine CLAP Audio Track" };

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
		static clap_host_state_t STATE_EXTENSION;
		static bool STATE_EXTENSION_INITIALIZED{ false };

		if (!STATE_EXTENSION_INITIALIZED)
		{
			STATE_EXTENSION.mark_dirty = [](const clap_host_t *host) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};

			STATE_EXTENSION_INITIALIZED = true;
		}

		return &STATE_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_NOTE_NAME))
	{
		static clap_host_note_name_t NOTE_NAME_EXTENSION;
		static bool NOTE_NAME_EXTENSION_INITIALIZED{ false };

		if (!NOTE_NAME_EXTENSION_INITIALIZED)
		{
			NOTE_NAME_EXTENSION.changed = [](const clap_host_t *host) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};

			NOTE_NAME_EXTENSION_INITIALIZED = true;
		}

		return &NOTE_NAME_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_REMOTE_CONTROLS) || StringUtilities::IsEqual(extension_id, CLAP_EXT_REMOTE_CONTROLS_COMPAT))
	{
		static clap_host_remote_controls_t REMOTE_CONTROLS_EXTENSION;
		static bool REMOTE_CONTROLS_EXTENSION_INITIALIZED{ false };

		if (!REMOTE_CONTROLS_EXTENSION_INITIALIZED)
		{
			REMOTE_CONTROLS_EXTENSION.changed = [](const clap_host_t *host) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};
			REMOTE_CONTROLS_EXTENSION.suggest_page = [](const clap_host_t *host, clap_id page_id) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};

			REMOTE_CONTROLS_EXTENSION_INITIALIZED = true;
		}

		return &REMOTE_CONTROLS_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_VOICE_INFO))
	{
		static clap_host_voice_info_t VOICE_INFO_EXTENSION;
		static bool VOICE_INFO_EXTENSION_INITIALIZED{ false };

		if (!VOICE_INFO_EXTENSION_INITIALIZED)
		{
			VOICE_INFO_EXTENSION.changed = [](const clap_host_t *host) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};

			VOICE_INFO_EXTENSION_INITIALIZED = true;
		}

		return &VOICE_INFO_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_CONTEXT_MENU) || StringUtilities::IsEqual(extension_id, CLAP_EXT_CONTEXT_MENU_COMPAT))
	{
		static clap_host_context_menu_t CONTEXT_MENU_EXTENSION;
		static bool CONTEXT_MENU_EXTENSION_INITIALIZED{ false };

		if (!CONTEXT_MENU_EXTENSION_INITIALIZED)
		{
			CONTEXT_MENU_EXTENSION.populate = [](const clap_host_t *host, const clap_context_menu_target_t *target, const clap_context_menu_builder_t *builder) -> bool
			{
				ASSERT(false, "Figure out what to do here!");
				return false;
			};
			CONTEXT_MENU_EXTENSION.perform = [](const clap_host_t *host, const clap_context_menu_target_t *target, clap_id action_id) -> bool
			{
				ASSERT(false, "Figure out what to do here!");
				return false;
			};
			CONTEXT_MENU_EXTENSION.can_popup = [](const clap_host_t *host) -> bool
			{
				ASSERT(false, "Figure out what to do here!");
				return false;
			};
			CONTEXT_MENU_EXTENSION.popup = [](const clap_host_t *host, const clap_context_menu_target_t *target, int32_t screen_index, int32_t x, int32_t y) -> bool
			{
				ASSERT(false, "Figure out what to do here!");
				return false;
			};

			CONTEXT_MENU_EXTENSION_INITIALIZED = true;
		}

		return &CONTEXT_MENU_EXTENSION;
	}

	else if (StringUtilities::IsEqual(extension_id, CLAP_EXT_PRESET_LOAD))
	{
		static clap_host_preset_load_t PRESET_LOAD_EXTENSION;
		static bool PRESET_LOAD_EXTENSION_INITIALIZED{ false };

		if (!PRESET_LOAD_EXTENSION_INITIALIZED)
		{
			PRESET_LOAD_EXTENSION.on_error = [](const clap_host_t *host, uint32_t location_kind, const char *location, const char *load_key, int32_t os_error, const char *msg) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};
			PRESET_LOAD_EXTENSION.loaded = [](const clap_host_t *host, uint32_t location_kind, const char *location, const char *load_key) -> void
			{
				ASSERT(false, "Figure out what to do here!");
			};

			PRESET_LOAD_EXTENSION_INITIALIZED = true;
		}

		return &PRESET_LOAD_EXTENSION;
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
		CLAPPlugin *const RESTRICT _host{ static_cast<CLAPPlugin *const RESTRICT>(host->host_data) };

		CLAPPlugin::MainThreadRequest request;

		request._Type = CLAPPlugin::MainThreadRequest::Type::UPDATE_PLUGIN;

		_host->AddMainThreadRequest(request);
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

#if ENABLE_GUI
	{
		const clap_plugin_gui_t *const RESTRICT plugin_gui{ static_cast<const clap_plugin_gui_t *const RESTRICT>(_Plugin->get_extension(_Plugin, CLAP_EXT_GUI)) };

		if (plugin_gui)
		{
			if (plugin_gui->is_api_supported(_Plugin, CLAP_WINDOW_API_WIN32, false))
			{
				if (plugin_gui->create(_Plugin, CLAP_WINDOW_API_WIN32, false))
				{
					uint32_t width;
					uint32_t height;

					if (!plugin_gui->get_size(_Plugin, &width, &height))
					{
						ASSERT(false, "Couldn't get size!");
					}

					{
						clap_window_t clap_window;

						clap_window.api = CLAP_WINDOW_API_WIN32;
						clap_window.win32 = CatalystPlatformWindows::_Window;

						if (!plugin_gui->set_parent(_Plugin, &clap_window))
						{
							ASSERT(false, "Couldn't set parent!");
						}
					}

					plugin_gui->suggest_title(_Plugin, "Catalyst Engine CLAP Window");

					if (!plugin_gui->show(_Plugin))
					{
						ASSERT(false, "Couldn't show!");
					}
				}
			}
		}
	}
#endif

	//Activate the plugin.
	if (!_Plugin->activate(_Plugin, static_cast<float64>(_SampleRate), 1, 4096))
	{
		ASSERT(false, "Could not retrieve CLAP entry!");

		Terminate();

		return false;
	}

	//Add the request to start processing.
	{
		AudioThreadRequest request;

		request._Type = AudioThreadRequest::Type::START_PROCESSING;

		AddAudioThreadRequest(request);
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

	//Initialization succeeded!
	return true;
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

		{
			const clap_plugin_gui_t *const RESTRICT plugin_gui{ static_cast<const clap_plugin_gui_t *const RESTRICT>(_Plugin->get_extension(_Plugin, CLAP_EXT_GUI)) };

			if (plugin_gui)
			{
				plugin_gui->destroy(_Plugin);
			}
		}

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
*	Updates this audio effect on the main thread.
*/
void CLAPPlugin::MainThreadUpdate() NOEXCEPT
{
	//Process requests.
	{
		Optional<MainThreadRequest> request{ _MainThreadRequests.Pop() };

		while (request.Valid())
		{
			const MainThreadRequest _request{ request.Get() };

			switch (_request._Type)
			{
				case MainThreadRequest::Type::NONE:
				{
					ASSERT(false, "Request must have a type!");

					break;
				}

				case MainThreadRequest::Type::UPDATE_PLUGIN:
				{
					_Plugin->on_main_thread(_Plugin);

					break;
				}

				case MainThreadRequest::Type::SHOW_GUI:
				{
					const clap_plugin_gui_t *const RESTRICT plugin_gui{ static_cast<const clap_plugin_gui_t *const RESTRICT>(_Plugin->get_extension(_Plugin, CLAP_EXT_GUI)) };

					if (plugin_gui)
					{
						plugin_gui->show(_Plugin);
					}

					break;
				}

				case MainThreadRequest::Type::HIDE_GUI:
				{


					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			request = _MainThreadRequests.Pop();
		}
	}
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

	//Process requests.
	{
		Optional<AudioThreadRequest> request{ _AudioThreadRequests.Pop() };

		while (request.Valid())
		{
			const AudioThreadRequest _request{ request.Get() };

			switch (_request._Type)
			{
				case AudioThreadRequest::Type::NONE:
				{
					ASSERT(false, "Request must have a type!");

					break;
				}

				case AudioThreadRequest::Type::START_PROCESSING:
				{
					_Plugin->start_processing(_Plugin);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			request = _AudioThreadRequests.Pop();
		}
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

	output_events.ctx = &_OutputEvents;
	output_events.try_push = [](const struct clap_output_events *list, const clap_event_header_t *event) -> bool
	{
		DynamicArray<const clap_event_header_t *RESTRICT> *const RESTRICT events{ static_cast<DynamicArray<const clap_event_header_t *RESTRICT> *const RESTRICT>(list->ctx) };
		events->Emplace(event);
		return true;
	};

	_Process.out_events = &output_events;

	//Process!
	const clap_process_status status{ _Plugin->process(_Plugin, &_Process) };

	//Process the output events.
	for (const clap_event_header_t *const RESTRICT output_event : _OutputEvents)
	{
		ASSERT(false, "Figure out what to do here!");
	}

	_OutputEvents.Clear();

	//Increment the steady time.
	_Process.steady_time += static_cast<int64_t>(number_of_samples);

	//Clear the input events.
	_InputEvents.Clear();
}

/*
*	Adds a main thread request.
*/
void CLAPPlugin::AddMainThreadRequest(const MainThreadRequest request) NOEXCEPT
{
	_MainThreadRequests.Push(request);
}

/*
*	Adds an audio thread request.
*/
void CLAPPlugin::AddAudioThreadRequest(const AudioThreadRequest request) NOEXCEPT
{
	_AudioThreadRequests.Push(request);
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