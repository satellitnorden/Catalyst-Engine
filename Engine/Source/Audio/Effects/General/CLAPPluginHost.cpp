//Header file.
#include <Audio/Effects/General/CLAPPluginHost.h>

//Core.
#include <Core/General/Print.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Concurrency.h>

//Platform.
#include <Platform/Core/DynamicLibrary.h>
#include <Platform/Core/WindowManager.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <ThirdParty/CLAP/ext/draft/resource-directory.h>
#include <ThirdParty/CLAP/clap.h>

/*
*	CLAP Parameter value change class definition.
*/
class CLAPParameterValueChange final
{

public:

	//The identifier.
	clap_id _Identifier;

	//The cookie.
	void *RESTRICT _Cookie;

	//The value.
	float64 _Value;

};

/*
*	CLAP event class definition.
*/
class CLAPEvent final
{

public:

	//Store all possible event types as a union.
	union
	{
		clap_event_param_value_t _ParameterValue;
	};

};

/*
*	CLAP plugin host implementation class definition.
*/
class CLAPPluginHostImplementation final
{

public:

	//The dynamic library.
	Platform::DynamicLibrary _DynamicLibrary;

	//The entry.
	const clap_plugin_entry *RESTRICT _Entry{ nullptr };

	//The host.
	clap_host_t _Host;

	//The plugin.
	const clap_plugin *RESTRICT _Plugin{ nullptr };

	//The host audio ports extension.
	clap_host_audio_ports _HostAudioPortsExtension;

	//The host GUI extension.
	clap_host_gui _HostGUIExtension;

	//The host log extension.
	clap_host_log _HostLogExtension;

	//The host thread check extension.
	clap_host_thread_check _HostThreadCheckExtension;

	//THe plugin GUI extension.
	const clap_plugin_gui *RESTRICT _PluginGUIExtension{ nullptr };

	//THe plugin params extension.
	const clap_plugin_params *RESTRICT _PluginParamsExtension{ nullptr };

	//The parent window.
	Platform::WindowHandle _ParentWindow{ Platform::INVALID_WINDOW_HANDLE };

	//Denotes whether or not the plugin is processing.
	bool _PluginIsProcessing{ false };

	//Denotes whether or not the plugin wants a main thread update.
	AtomicFlag _PluginWantsMainThreadUpdate;

	//The incoming parameter value changes.
	AtomicQueue<CLAPParameterValueChange, 64, AtomicQueueMode::MULTIPLE, AtomicQueueMode::MULTIPLE> _IncomingParameterValueChanges;

	//The input events.
	DynamicArray<CLAPEvent> _InputEvents;

	//The output events.
	DynamicArray<CLAPEvent> _OutputEvents;

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The process.
	clap_process _Process;

};

constexpr size_t CLAP_PLUGIN_HOST_IMPLEMENTATION_SIZE{ sizeof(CLAPPluginHostImplementation) };
static_assert(CLAP_PLUGIN_HOST_IMPLEMENTATION_SIZE <= CLAPPluginHost::IMPLEMENTATION_SIZE, "Increase implementation size!");

/*
*	Default constructor.
*/
CLAPPluginHost::CLAPPluginHost() NOEXCEPT
{
	//Construct the implementation.
	new (Implementation()) CLAPPluginHostImplementation();

	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Reset the process struct.
	Memory::Set(&implementation->_Process, 0, sizeof(clap_process));
}

/*
*	Default destructor.
*/
CLAPPluginHost::~CLAPPluginHost() NOEXCEPT
{
	//Terminate.
	Terminate();

	//Call the destructor.
	Implementation()->~CLAPPluginHostImplementation();
}

/*
*	Updates this audio effect on the main thread.
*/
void CLAPPluginHost::MainThreadUpdate() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Update the plugin, if requested.
	if (implementation->_PluginWantsMainThreadUpdate.IsSet())
	{
		implementation->_Plugin->on_main_thread(implementation->_Plugin);

		implementation->_PluginWantsMainThreadUpdate.Clear();
	}
}

/*
*	Initializes this plugin host. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Load the dynamic library.
	if (!implementation->_DynamicLibrary.Load(plugin_file_path))
	{
		LOG_ERROR("Couldn't load CLAP plugin '%s'!", plugin_file_path);
		return false;
	}

	//Try to initialize the dynamic library.
	if (!implementation->_DynamicLibrary.Initialize())
	{
		ASSERT(false, "Could not initialize the module!");
		Terminate();
		return false;
	}

	//Retrieve the CLAP entry.
	implementation->_Entry = reinterpret_cast<const clap_plugin_entry *const RESTRICT>(implementation->_DynamicLibrary.GetSymbol("clap_entry"));

	if (!implementation->_Entry)
	{
		ASSERT(false, "Could not retrieve 'clap_entry' entry point!");
		Terminate();
		return false;
	}

	if (!implementation->_Entry->init)
	{
		ASSERT(false, "'clap_entry' entry point does not have a 'init' function!");
		Terminate();
		return false;
	}

	if (!implementation->_Entry->init(plugin_file_path))
	{
		ASSERT(false, "Could not initialize 'clap_entry' entry point!");
		Terminate();
		return false;
	}

	//Fill in the host.
	implementation->_Host.clap_version = CLAP_VERSION;
	implementation->_Host.host_data = this;
	implementation->_Host.name = "Catalyst Engine";
	implementation->_Host.vendor = "Daybreak Studio";
	implementation->_Host.url = "CatalystEngine.com";
	implementation->_Host.version = "1.0";
	implementation->_Host.get_extension = [](const clap_host_t *host, const char *extension_id) -> const void*
	{
		//Cache the CLAP plugin host.
		CLAPPluginHost *const RESTRICT clap_plugin_host{ static_cast<CLAPPluginHost* const RESTRICT>(host->host_data) };

		//Retrieve the extension.
		return clap_plugin_host->GetExtension(extension_id);
	};
	implementation->_Host.request_restart = [](const clap_host_t *host)
	{
		ASSERT(false, "Implement this!");
	};
	implementation->_Host.request_process = [](const clap_host_t *host)
	{
		ASSERT(false, "Implement this!");
	};
	implementation->_Host.request_callback = [](const clap_host_t *host)
	{
		//Cache the CLAP plugin host.
		CLAPPluginHost *const RESTRICT clap_plugin_host{ static_cast<CLAPPluginHost *const RESTRICT>(host->host_data) };

		//Cache the implementation.
		CLAPPluginHostImplementation *const RESTRICT implementation{ clap_plugin_host->Implementation() };

		//Signal that the plugin wants a main thread update.
		implementation->_PluginWantsMainThreadUpdate.Set();
	};

	//Try to retrieve the 'get_factory' function.
	if (!implementation->_Entry->get_factory)
	{
		ASSERT(false, "'clap_entry' entry point does not expose 'get_factory' function!");
		Terminate();
		return false;
	}

	const clap_plugin_factory *const RESTRICT factory{ static_cast<const clap_plugin_factory *const RESTRICT>(implementation->_Entry->get_factory(CLAP_PLUGIN_FACTORY_ID)) };

	if (!factory)
	{
		ASSERT(false, "Could not retrieve factory!");
		Terminate();
		return false;
	}

	//Retrieve the number of plugins.
	const uint32 number_of_plugins{ factory->get_plugin_count(factory) };

	if (number_of_plugins != 1)
	{
		ASSERT(false, "Number of plugins in CLAP plugin is not 1!");
		Terminate();
		return false;
	}

	//Retrieve the plugin descriptor.
	const clap_plugin_descriptor *const RESTRICT plugin_descriptor{ factory->get_plugin_descriptor(factory, 0) };

	if (!plugin_descriptor)
	{
		ASSERT(false, "Could not retrieve plugin descriptor!");
		Terminate();
		return false;
	}

	//Create the plugin.
	implementation->_Plugin = factory->create_plugin(factory, &implementation->_Host, plugin_descriptor->id);

	if (!implementation->_Plugin)
	{
		ASSERT(false, "Could not create plugin!");
		Terminate();
		return false;
	}

	//Initialize the plugin.
	if (!implementation->_Plugin->init(implementation->_Plugin))
	{
		ASSERT(false, "Could not initialize plugin!");
		Terminate();
		return false;
	}

	//Activate the plugin.
	if (!implementation->_Plugin->activate(implementation->_Plugin, static_cast<float64>(_SampleRate), 1, 4'096))
	{
		ASSERT(false, "Could not initialize plugin!");
		Terminate();
		return false;
	}

	//Try to retrieve the plugin GUI extension.
	implementation->_PluginGUIExtension = static_cast<const clap_plugin_gui *const RESTRICT>(implementation->_Plugin->get_extension(implementation->_Plugin, CLAP_EXT_GUI));

	//Retrieve the parameters.
	if (implementation->_PluginParamsExtension = static_cast<const clap_plugin_params *const RESTRICT>(implementation->_Plugin->get_extension(implementation->_Plugin, CLAP_EXT_PARAMS)))
	{
		//Retrieve the number of parameters.
		const uint32 number_of_parameters{ implementation->_PluginParamsExtension->count(implementation->_Plugin) };
		_Parameters.Reserve(number_of_parameters);

		for (uint32 parameter_index{ 0 }; parameter_index < number_of_parameters; ++parameter_index)
		{
			clap_param_info parameter_info;
			
			if (!implementation->_PluginParamsExtension->get_info(implementation->_Plugin, parameter_index, &parameter_info))
			{
				continue;
			}

			_Parameters.Emplace();
			Parameter &new_parameter{ _Parameters.Back() };

			new_parameter._Index = parameter_index;
			SNPRINTF(new_parameter._Name.Data(), new_parameter._Name.Size(), "%s", parameter_info.name);
			new_parameter._Identifier = HashString(new_parameter._Name.Data());
			new_parameter._DefaultPlainValue = new_parameter._CurrentPlainValue = parameter_info.default_value;
			new_parameter._DefaultNormalizedValue = new_parameter._CurrentNormalizedValue = BaseMath::Scale(parameter_info.default_value, parameter_info.min_value, parameter_info.max_value, 0.0, 1.0);
			new_parameter._FormatIdentifier._uint32 = parameter_info.id;
		}
	}

	//We are now initialized!
	_Initialized = true;

	return true;
}

/*
*	Returns the parameter (plain) with the given identifier. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::GetParameterPlain(const HashString identifier, float64 *const RESTRICT value) NOEXCEPT
{
	//Can't set parameters if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Find the parameter.
	const Parameter *const RESTRICT parameter{ FindParameter(identifier) };

	if (!parameter)
	{
		ASSERT(false, "Could not find parameter!");

		return false;
	}

	//Set the value.
	*value = parameter->_CurrentPlainValue;

	//Return success.
	return true;
}

/*
*	Sets a parameter (plain) with the given identifier. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::SetParameterPlain(const HashString identifier, const float64 value) NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Can't set parameters if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Find the parameter.
	Parameter *const RESTRICT parameter{ FindParameter(identifier) };

	if (!parameter)
	{
		//ASSERT(false, "Could not find parameter!");

		return false;
	}

	//Retrieve the parameter info.
	clap_param_info parameter_info;

	if (!implementation->_PluginParamsExtension->get_info(implementation->_Plugin, parameter->_Index, &parameter_info))
	{
		return false;
	}

	//Set the value.
	parameter->_CurrentPlainValue = value;
	parameter->_CurrentNormalizedValue = (value - parameter_info.min_value) / (parameter_info.max_value - parameter_info.min_value);

	//Add the incoming parameter value change.
	implementation->_IncomingParameterValueChanges.Push(CLAPParameterValueChange{ parameter_info.id, parameter_info.cookie, parameter->_CurrentPlainValue });

	//Return success.
	return true;
}

/*
*	Returns the parameter (normalized) with the given identifier. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::GetParameterNormalized(const HashString identifier, float64 *const RESTRICT value) NOEXCEPT
{
	//Can't set parameters if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Find the parameter.
	const Parameter *const RESTRICT parameter{ FindParameter(identifier) };

	if (!parameter)
	{
		ASSERT(false, "Could not find parameter!");

		return false;
	}

	//Set the value.
	*value = parameter->_CurrentNormalizedValue;

	//Return success.
	return true;
}

/*
*	Sets a parameter (normalized) with the given identifier. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::SetParameterNormalized(const HashString identifier, const float64 value) NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Can't set parameters if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Find the parameter.
	Parameter *const RESTRICT parameter{ FindParameter(identifier) };

	if (!parameter)
	{
		ASSERT(false, "Could not find parameter!");

		return false;
	}

	//Retrieve the parameter info.
	clap_param_info parameter_info;

	if (!implementation->_PluginParamsExtension->get_info(implementation->_Plugin, parameter->_Index, &parameter_info))
	{
		return false;
	}

	//Set the value.
	parameter->_CurrentPlainValue = BaseMath::LinearlyInterpolate(parameter_info.min_value, parameter_info.max_value, value);
	parameter->_CurrentNormalizedValue = value;

	//Add the incoming parameter value change.
	implementation->_IncomingParameterValueChanges.Push(CLAPParameterValueChange{ parameter_info.id, parameter_info.cookie, parameter->_CurrentPlainValue });

	//Return success.
	return true;
}

/*
*	Shows the UI. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::ShowUI() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Can't show UI if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Can't proceed if we don't have the plugin GUI extension.
	if (!implementation->_PluginGUIExtension)
	{
		return false;
	}

	//Check if the preferred API is supported.
	if (!implementation->_PluginGUIExtension->is_api_supported(implementation->_Plugin, CLAP_WINDOW_API_WIN32, false))
	{
		return false;
	}

	//Try to create the GUI.
	if (!implementation->_PluginGUIExtension->create(implementation->_Plugin, CLAP_WINDOW_API_WIN32, false))
	{
		return false;
	}

	//Try to retrieve the resolution.
	uint32 width;
	uint32 height;

	if (!implementation->_PluginGUIExtension->get_size(implementation->_Plugin, &width, &height))
	{
		return false;
	}

	//Create the parent window.
	{
		Platform::WindowManager::CreateParameters parameters;

		parameters._Title = "TODO: Show name of plugin here.";
		parameters._Resolution = Resolution(width, height);
		parameters._FullScreen = false;
		parameters._ResolutionIncludesPlatformElements = false;
		parameters._SupportOpenGL = false;

		implementation->_ParentWindow = Platform::WindowManager::Create(parameters);
	}

	//Set the parent window.
	{
		clap_window window;

		window.api = CLAP_WINDOW_API_WIN32;
		window.win32 = Platform::WindowManager::GetWindowHandle(implementation->_ParentWindow);

		if (!implementation->_PluginGUIExtension->set_parent(implementation->_Plugin, &window))
		{
			return false;
		}
	}

	//Show the GUI!
	if (!implementation->_PluginGUIExtension->show(implementation->_Plugin))
	{
		return false;
	}

	//We should have succeeded by now. (:
	return true;
}

/*
*	Hides the UI. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::HideUI() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Destroy the plugin GUI extension.
	if (implementation->_PluginGUIExtension)
	{
		implementation->_PluginGUIExtension->destroy(implementation->_Plugin);
		implementation->_PluginGUIExtension = nullptr;
	}

	//Destroy the parent window.
	if (implementation->_ParentWindow != Platform::INVALID_WINDOW_HANDLE)
	{
		Platform::WindowManager::Destroy(implementation->_ParentWindow);
		implementation->_ParentWindow = Platform::INVALID_WINDOW_HANDLE;
	}

	//Return success.
	return true;
}

/*
*	Returns the state. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::GetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT
{
	return false;
}

/*
*	Sets the state. Returns if it succeeded.
*/
NO_DISCARD bool CLAPPluginHost::SetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT
{
	return false;
}

/*
*	Terminates this VST3 plugin.
*/
void CLAPPluginHost::Terminate() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Terminate the plugin.
	if (implementation->_Plugin)
	{
		//Tell the plugin to stop processing.
		implementation->_Plugin->stop_processing(implementation->_Plugin);

		//Deactivate the plugin.
		implementation->_Plugin->deactivate(implementation->_Plugin);

		//Hide the UI.
		HideUI();

		//Destroy the plugin.
		implementation->_Plugin->destroy(implementation->_Plugin);

		//Reset the plugin.
		implementation->_Plugin = nullptr;
	}

	//Deinitialize the entry.
	if (implementation->_Entry && implementation->_Entry->deinit)
	{
		implementation->_Entry->deinit();
		implementation->_Entry = nullptr;
	}

	//Unload the dynamic library.
	implementation->_DynamicLibrary.Unload();
}

/*
*	Callback for when this plugin receives MIDI messages.
*/
void CLAPPluginHost::OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT
{
	
}

/*
*	Callback for this audio effect to process the given buffer.
*/
void CLAPPluginHost::Process
(
	const AudioProcessContext &context,
	const DynamicArray<DynamicArray<float32>> &inputs,
	DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
	const uint8 number_of_channels,
	const uint32 number_of_samples
) NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//If we don't have a plugin, just copy inputs into outputs and be done with it.
	if (!implementation->_Plugin)
	{
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		return;
	}

	//Start processing on the plugin if it is not processing yet.
	if (!implementation->_PluginIsProcessing)
	{
		if (!implementation->_Plugin->start_processing(implementation->_Plugin))
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
			}

			return;
		}

		implementation->_PluginIsProcessing = true;
	}

	//Set up the input buffer.
	implementation->_InputBuffer.Resize<true>(number_of_channels);

	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		implementation->_InputBuffer[channel_index].Resize<false>(number_of_samples);
		Memory::Copy(implementation->_InputBuffer[channel_index].Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
	}

	//Process the incoming parameter value changes.
	ProcessIncomingParameterValueChanges();

	//Set up the process struct.
	implementation->_Process.frames_count = number_of_samples;
	implementation->_Process.transport = nullptr; //TODO!

	float32 *RESTRICT input_data_pointers[]
	{
		implementation->_InputBuffer[0].Data(),
		implementation->_InputBuffer[1].Data()
	};

	clap_audio_buffer_t input_buffer;

	input_buffer.data32 = input_data_pointers;
	input_buffer.data64 = nullptr;
	input_buffer.channel_count = number_of_channels;
	input_buffer.latency = 0; //I think?
	input_buffer.constant_mask = 0;

	implementation->_Process.audio_inputs = &input_buffer;

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

	implementation->_Process.audio_outputs = &output_buffer;

	implementation->_Process.audio_inputs_count = 1;
	implementation->_Process.audio_outputs_count = 1;

	clap_input_events input_events;

	input_events.ctx = &implementation->_InputEvents;
	input_events.size = [](const clap_input_events *list) -> uint32_t
	{
		const DynamicArray<CLAPEvent> *const RESTRICT events{ static_cast<const DynamicArray<CLAPEvent> *const RESTRICT>(list->ctx) };
		return static_cast<uint32_t>(events->Size());
	};
	input_events.get = [](const clap_input_events *list, uint32_t index) -> const clap_event_header_t*
	{
		const DynamicArray<CLAPEvent> *const RESTRICT events{ static_cast<const DynamicArray<CLAPEvent> *const RESTRICT>(list->ctx) };
		return reinterpret_cast<const clap_event_header_t *const RESTRICT>(&events->At(index));
	};

	implementation->_Process.in_events = &input_events;

	clap_output_events output_events;

	output_events.ctx = &implementation->_OutputEvents;
	output_events.try_push = [](const struct clap_output_events *list, const clap_event_header_t *event)
	{
		const DynamicArray<CLAPEvent> *const RESTRICT events{ static_cast<const DynamicArray<CLAPEvent> *const RESTRICT>(list->ctx) };

		switch (event->type)
		{
			case CLAP_EVENT_PARAM_VALUE:
			case CLAP_EVENT_PARAM_GESTURE_BEGIN:
			case CLAP_EVENT_PARAM_GESTURE_END:
			{
				//Do nothing. (:

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		return true;
	};

	implementation->_Process.out_events = &output_events;

	//Process!
	const clap_process_status status{ implementation->_Plugin->process(implementation->_Plugin, &implementation->_Process) };

	//Clear the input events.
	implementation->_InputEvents.Clear();

	//Increment the steady time.
	implementation->_Process.steady_time += static_cast<int64_t>(number_of_samples);
}

/*
*	Returns the implementation.
*/
NO_DISCARD CLAPPluginHostImplementation *const RESTRICT CLAPPluginHost::Implementation() NOEXCEPT
{
	return _Implementation.Get<CLAPPluginHostImplementation>();
}

/*
*	Returns the extension with the given identifier (if supported)
*/
NO_DISCARD const void *const RESTRICT CLAPPluginHost::GetExtension(const char *const RESTRICT identifier) NOEXCEPT
{
	if (StringUtilities::IsEqual(identifier, CLAP_EXT_AUDIO_PORTS))
	{
		return GetAudioPortsExtension();
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_AUDIO_PORTS_CONFIG))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_CONTEXT_MENU))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_CONTEXT_MENU_COMPAT))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_GUI))
	{
		return GetGUIExtension();
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_LATENCY))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_LOG))
	{
		return GetLogExtension();
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_NOTE_NAME))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_NOTE_PORTS))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_PARAMS))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_POSIX_FD_SUPPORT))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_PRESET_LOAD))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_REMOTE_CONTROLS))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_REMOTE_CONTROLS_COMPAT))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_RESOURCE_DIRECTORY))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_STATE))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_THREAD_CHECK))
	{
		return GetThreadCheckExtension();
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_THREAD_POOL))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_TIMER_SUPPORT))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_TRACK_INFO))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_TRACK_INFO_COMPAT))
	{
		//Intentionally not supported.
		return nullptr;
	}

	if (StringUtilities::IsEqual(identifier, CLAP_EXT_VOICE_INFO))
	{
		//Intentionally not supported.
		return nullptr;
	}

	ASSERT(false, "CLAP extension '%s' is not supported!", identifier);

	return nullptr;
}

/*
*	Returns the audio ports extension.
*/
NO_DISCARD const void *const RESTRICT CLAPPluginHost::GetAudioPortsExtension() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Set up the extension.
	implementation->_HostAudioPortsExtension.is_rescan_flag_supported = [](const clap_host_t* host, uint32_t flag)
	{
		ASSERT(false, "Implement this!");

		return false;
	};
	implementation->_HostAudioPortsExtension.rescan = [](const clap_host_t* host, uint32_t flags)
	{
		ASSERT(false, "Implement this!");
	};

	//Return the extension.
	return &implementation->_HostAudioPortsExtension;
}

/*
*	Returns the GUI extension.
*/
NO_DISCARD const void *const RESTRICT CLAPPluginHost::GetGUIExtension() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Set up the extension.
	implementation->_HostGUIExtension.resize_hints_changed = [](const clap_host_t *host)
	{
		//Nothing to do here. (:
	};
	implementation->_HostGUIExtension.request_resize = [](const clap_host_t *host, uint32_t width, uint32_t height)
	{
		//Not supported right now.
		return false;
	};
	implementation->_HostGUIExtension.request_show = [](const clap_host_t *host)
	{
		//Not supported right now.
		return false;
	};
	implementation->_HostGUIExtension.request_hide = [](const clap_host_t *host)
	{
		//Not supported right now.
		return false;
	};
	implementation->_HostGUIExtension.closed = [](const clap_host_t *host, bool was_destroyed)
	{
		//Nothing to do here. (:
	};

	//Return the extension.
	return &implementation->_HostGUIExtension;
}

/*
*	Returns the log extension.
*/
NO_DISCARD const void *const RESTRICT CLAPPluginHost::GetLogExtension() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Set up the extension.
	implementation->_HostLogExtension.log = [](const clap_host_t *host, clap_log_severity severity, const char *msg)
	{
		switch (severity)
		{
			case CLAP_LOG_DEBUG:
			{
				LOG_DEBUG(msg);

				break;
			}

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

	//Return the extension.
	return &implementation->_HostLogExtension;
}

/*
*	Returns the thread check extension.
*/
NO_DISCARD const void *const RESTRICT CLAPPluginHost::GetThreadCheckExtension() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Set up the extension.
	implementation->_HostThreadCheckExtension.is_main_thread = [](const clap_host_t *host)
	{
		return Concurrency::CurrentThread::IsMainThread();
	};
	implementation->_HostThreadCheckExtension.is_audio_thread = [](const clap_host_t* host)
	{
		//It is assumed that any thread this plugin runs on that is not the main thread is an audio thread.
		return !Concurrency::CurrentThread::IsMainThread();
	};

	//Return the extension.
	return &implementation->_HostThreadCheckExtension;
}

/*
*	Processes the incoming parameter value changes.
*/
void CLAPPluginHost::ProcessIncomingParameterValueChanges() NOEXCEPT
{
	//Cache the implementation.
	CLAPPluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Process the incoming parameter value change.
	Optional<CLAPParameterValueChange> incoming_parameter_value_change{ implementation->_IncomingParameterValueChanges.Pop() };

	while (incoming_parameter_value_change.Valid())
	{
		const CLAPParameterValueChange &_incoming_parameter_value_change{ incoming_parameter_value_change.Get() };

		implementation->_InputEvents.Emplace();
		CLAPEvent &input_event{ implementation->_InputEvents.Back() };

		input_event._ParameterValue.header.size = static_cast<uint32_t>(sizeof(clap_event_param_value_t));
		input_event._ParameterValue.header.time = 0;
		input_event._ParameterValue.header.space_id = 0; //TODO: Figure out what this does?
		input_event._ParameterValue.header.type = CLAP_EVENT_PARAM_VALUE;
		input_event._ParameterValue.header.flags = 0; //TODO: Figure out if this should have any flag?

		input_event._ParameterValue.param_id = _incoming_parameter_value_change._Identifier;
		input_event._ParameterValue.cookie = _incoming_parameter_value_change._Cookie;

		input_event._ParameterValue.note_id = -1;
		input_event._ParameterValue.port_index = -1;
		input_event._ParameterValue.channel = -1;
		input_event._ParameterValue.key = -1;

		input_event._ParameterValue.value = _incoming_parameter_value_change._Value;

		incoming_parameter_value_change = implementation->_IncomingParameterValueChanges.Pop();
	}
}