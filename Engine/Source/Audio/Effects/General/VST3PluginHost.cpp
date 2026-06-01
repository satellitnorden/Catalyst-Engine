//Header file.
#include <Audio/Effects/General/VST3PluginHost.h>

//Core.
#include <Core/General/DynamicLibrary.h>
#include <Core/Utilities/StringUtilities.h>

//Audio.
#include <Audio/VST3/BitStream.h>
#include <Audio/VST3/ComponentHandler.h>
#include <Audio/VST3/HostApplication.h>
#include <Audio/VST3/PlugFrame.h>
#include <Audio/VST3/Utilities.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <VST3/pluginterfaces/base/ipluginbase.h>
#include <VST3/pluginterfaces/gui/iplugview.h>
#include <VST3/pluginterfaces/vst/ivstaudioprocessor.h>
#include <VST3/pluginterfaces/vst/ivsteditcontroller.h>
#include <VST3/pluginterfaces/vst/ivsthostapplication.h>
#include <VST3/pluginterfaces/vst/ivstprocesscontext.h>

//STD.
#include <atomic>

//Type aliases.
using InitModuleProc = bool(*)();
using ExitModuleProc = bool(*)();

/*
*	VST3 plugin host implementation class definition.
*/
class VST3PluginHostImplementation final
{

public:

	//The host application.
	VST3::HostApplication _HostApplication;

	//The plug frame.
	VST3::PlugFrame _PlugFrame;

	//The component handler.
	VST3::ComponentHandler _ComponentHandler;

	//The dynamic library.
	DynamicLibrary _DynamicLibrary;

	//The plugin factory.
	Steinberg::IPluginFactory *RESTRICT _PluginFactory{ nullptr };

	//The audio effect component.
	Steinberg::Vst::IComponent *RESTRICT _AudioEffectComponent{ nullptr };

	//The component connection point.
	Steinberg::Vst::IConnectionPoint *RESTRICT _ComponentConnectionPoint{ nullptr };

	//The audio edit controller.
	Steinberg::Vst::IEditController *RESTRICT _EditController{ nullptr };

	//The controller connection point.
	Steinberg::Vst::IConnectionPoint *RESTRICT _ControllerConnectionPoint{ nullptr };

	//The audio processor component.
	Steinberg::Vst::IAudioProcessor *RESTRICT _AudioProcessor{ nullptr };

	//The editor view.
	Steinberg::IPlugView *RESTRICT _EditorView{ nullptr };

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The output buffer.
	DynamicArray<DynamicArray<float32>> _OutputBuffer;

	//The project time.
	int64 _ProjectTime{ 0 };

};

constexpr size_t VST3_PLUGIN_HOST_IMPLEMENTATION_SIZE{ sizeof(VST3PluginHostImplementation) };
static_assert(VST3_PLUGIN_HOST_IMPLEMENTATION_SIZE <= VST3PluginHost::IMPLEMENTATION_SIZE, "Increase implementation size!");

/*
*	Default constructor.
*/
VST3PluginHost::VST3PluginHost() NOEXCEPT
{
	//Construct the implementation.
	new (Implementation()) VST3PluginHostImplementation();
}

/*
*	Default destructor.
*/
VST3PluginHost::~VST3PluginHost() NOEXCEPT
{
	//Terminate.
	Terminate();

	//Call the destructor.
	Implementation()->~VST3PluginHostImplementation();
}

/*
*	Initializes this plugin host. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Keep a result variable around.
	Steinberg::tresult result{ Steinberg::kResultOk };

	//Load the dynamic library.
	if (!implementation->_DynamicLibrary.Load(plugin_file_path))
	{
		ASSERT(false, "Couldn't load %s!", plugin_file_path);
		return false;
	}

	//Retrieve the init module function.
	InitModuleProc init_module_function{ static_cast<InitModuleProc>(implementation->_DynamicLibrary.GetSymbol("InitDll")) };

	if (!init_module_function)
	{
		init_module_function = static_cast<InitModuleProc>(implementation->_DynamicLibrary.GetSymbol("ModuleEntry"));
	}

	if (init_module_function && !init_module_function())
	{
		ASSERT(false, "Could not initialize the module!");
		Terminate();
		return false;
	}

	//Retrieve the "GetPluginFactory()" function.
	GetFactoryProc get_plugin_factory_function{ static_cast<GetFactoryProc>(implementation->_DynamicLibrary.GetSymbol("GetPluginFactory")) };

	if (!get_plugin_factory_function)
	{
		ASSERT(false, "Could not retrieve \"GetPluginFactory()\" function!");
		Terminate();
		return false;
	}

	//Retrieve the plugin factory.
	implementation->_PluginFactory = get_plugin_factory_function();

	if (!implementation->_PluginFactory)
	{
		ASSERT(false, "Could not retrieve plugin factory!");
		Terminate();
		return false;
	}

	//Retrieve the factory info.
	Steinberg::PFactoryInfo factory_info;

	if (implementation->_PluginFactory->getFactoryInfo(&factory_info) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not retrieve factory info!");
		Terminate();
		return false;
	}

	//Retrieve the number of classes.
	const int32 number_of_classes{ implementation->_PluginFactory->countClasses() };

	//Iterate over the classes.
	bool found_audio_effect_class{ false };

	for (int32 class_index{ 0 }; class_index < number_of_classes; ++class_index)
	{
		//Retrieve the class info.
		Steinberg::PClassInfo class_info;

		if (implementation->_PluginFactory->getClassInfo(class_index, &class_info) != Steinberg::kResultOk)
		{
			ASSERT(false, "Could not retrieve class info!");
			Terminate();
			return false;
		}

		//Is this the audio effect class?
		if (StringUtilities::IsEqual(class_info.category, kVstAudioEffectClass))
		{
			//Set the plugin name.
			ASSERT(_PluginName.Size() >= ARRAY_LENGTH(class_info.name), "Not enough space to store the plugin name!");
			Memory::Copy(_PluginName.Data(), class_info.name, _PluginName.Size());

			result = implementation->_PluginFactory->createInstance
			(
				class_info.cid,
				Steinberg::Vst::IComponent_iid,
				reinterpret_cast<void**>(&implementation->_AudioEffectComponent)
			);

			if (result != Steinberg::kResultOk)
			{
				ASSERT(false, "Couldn't create audio effect component!");
				Terminate();
				return false;
			}

			//Initialize the audio effect component.
			if (implementation->_AudioEffectComponent->initialize(&implementation->_HostApplication) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not initialize audio effect component!");
				Terminate();
				return false;
			}

			//Create the component connection point.
			if (implementation->_AudioEffectComponent->queryInterface(Steinberg::Vst::IConnectionPoint_iid, reinterpret_cast<void**>(&implementation->_ComponentConnectionPoint)) != Steinberg::kResultOk)
			{
				//Not every plugin exposes a connection point on the component, and that's okay - Still, set it to 'nullptr' in case the plugin did anything weird.
				implementation->_ComponentConnectionPoint = nullptr;
			}

			//Create the edit controller.
			if (!CreateEditController())
			{
				ASSERT(false, "Could not create edit controller!");
				Terminate();
				return false;
			}

			//Initialize the edit controller.
			if (!InitializeEditController())
			{
				ASSERT(false, "Could not initialize edit controller!");
				Terminate();
				return false;
			}

			//Create the controller connection point.
			if (implementation->_EditController->queryInterface(Steinberg::Vst::IConnectionPoint_iid, reinterpret_cast<void**>(&implementation->_ControllerConnectionPoint)) != Steinberg::kResultOk)
			{
				//Not every plugin exposes a connection point on the edit controller, and that's okay - Still, set it to 'nullptr' in case the plugin did anything weird.
				implementation->_ControllerConnectionPoint = nullptr;
			}

			//Connect the component and the controller.
			if (implementation->_ComponentConnectionPoint && implementation->_ControllerConnectionPoint)
			{
				if (implementation->_ComponentConnectionPoint->connect(implementation->_ControllerConnectionPoint) != Steinberg::kResultOk)
				{
					ASSERT(false, "Could not connect controller to component!");
					Terminate();
					return false;
				}

				if (implementation->_ControllerConnectionPoint->connect(implementation->_ComponentConnectionPoint) != Steinberg::kResultOk)
				{
					ASSERT(false, "Could not connect component to controller!");
					Terminate();
					return false;
				}
			}

			//Set up the parameters.
			{
				const int32 number_of_parameters{ implementation->_EditController->getParameterCount() };
				_Parameters.Reserve(number_of_parameters);

				for (int32 parameter_index{ 0 }; parameter_index < number_of_parameters; ++parameter_index)
				{
					Steinberg::Vst::ParameterInfo parameter_info;

					if (implementation->_EditController->getParameterInfo(parameter_index, parameter_info) != Steinberg::kResultOk)
					{
						ASSERT(false, "Could not retrieve parameter from edit controller at index %i", parameter_index);
						Terminate();
						return false;
					}

					_Parameters.Emplace();
					Parameter &new_parameter{ _Parameters.Back() };

					VST3::Utilities::ConvertString128(parameter_info.title, &new_parameter._Name);
					new_parameter._Identifier = HashString(new_parameter._Name.Data());
					new_parameter._DefaultNormalizedValue = new_parameter._CurrentNormalizedValue = parameter_info.defaultNormalizedValue;
					new_parameter._DefaultPlainValue = new_parameter._CurrentPlainValue = implementation->_EditController->normalizedParamToPlain(parameter_info.id, parameter_info.defaultNormalizedValue);
					new_parameter._FormatIdentifier._uint32 = parameter_info.id;
				}
			}

			//Set the parameters on the component handler.
			implementation->_ComponentHandler.SetParameters(&_Parameters);

			//Query the bus information.
			bool has_stereo_input{ false };
			bool has_stereo_output{ false };

			{
				//Query the input buses.
				const int32 input_bus_count{ implementation->_AudioEffectComponent->getBusCount(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kInput) };

				for (int32 input_bus_index{ 0 }; input_bus_index < input_bus_count; ++input_bus_index)
				{
					Steinberg::Vst::BusInfo bus_info;

					if (implementation->_AudioEffectComponent->getBusInfo(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kInput, input_bus_index, bus_info) != Steinberg::kResultOk)
					{
						ASSERT(false, "Could not retrieve bus info for input bus at index %i!", input_bus_index);
					}

					else
					{
						if (bus_info.channelCount == 2)
						{
							has_stereo_input = true;

							break;
						}
					}
				}

				//Query the output buses.
				const int32 output_bus_count{ implementation->_AudioEffectComponent->getBusCount(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kOutput) };

				for (int32 output_bus_index{ 0 }; output_bus_index < output_bus_count; ++output_bus_index)
				{
					Steinberg::Vst::BusInfo bus_info;

					if (implementation->_AudioEffectComponent->getBusInfo(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kOutput, output_bus_index, bus_info) != Steinberg::kResultOk)
					{
						ASSERT(false, "Could not retrieve bus info for output bus at index %i!", output_bus_count);
					}

					else
					{
						if (bus_info.channelCount == 2)
						{
							has_stereo_output = true;

							break;
						}
					}
				}
			}

			if (!has_stereo_input || !has_stereo_output)
			{
				ASSERT(false, "We assume stereo input and output, handle this!");
				Terminate();
				return false;
			}

			//Create the audio processor.
			result = implementation->_AudioEffectComponent->queryInterface
			(
				Steinberg::Vst::IAudioProcessor_iid,
				reinterpret_cast<void**>(&implementation->_AudioProcessor)
			);

			if (result != Steinberg::kResultOk)
			{
				ASSERT(false, "Couldn't retrieve audio processor!");
				Terminate();
				return false;
			}

			//Set the bus arrangements.
			{
				Steinberg::Vst::SpeakerArrangement INPUT_ARRANGEMENT{ Steinberg::Vst::SpeakerArr::kStereo };
				Steinberg::Vst::SpeakerArrangement OUTPUT_ARRANGEMENT{ Steinberg::Vst::SpeakerArr::kStereo };

				if (implementation->_AudioProcessor->setBusArrangements(&INPUT_ARRANGEMENT, 1, &OUTPUT_ARRANGEMENT, 1) != Steinberg::kResultOk)
				{
					ASSERT(false, "Could not set bus arrangements on the audio processor!");
					Terminate();
					return false;
				}
			}

			//Activate the buses.
			if (implementation->_AudioEffectComponent->activateBus(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kInput, 0, true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not activate input bus on audio effect component!");
				Terminate();
				return false;
			}

			if (implementation->_AudioEffectComponent->activateBus(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kOutput, 0, true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not activate output bus on audio effect component!");
				Terminate();
				return false;
			}

			//Activate the audio effect component.
			if (implementation->_AudioEffectComponent->setActive(true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not activate audio effect component!");
				Terminate();
				return false;
			}

			//Setup the processing on the audio processor.
			{
				Steinberg::Vst::ProcessSetup setup;

				setup.processMode = Steinberg::Vst::ProcessModes::kRealtime;
				setup.symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample32;
				setup.maxSamplesPerBlock = 4'096; //TODO: Think about this. (:
				setup.sampleRate = static_cast<Steinberg::Vst::SampleRate>(Audio::DEFAULT_SAMPLE_RATE); //TODO: Should make this dynamic!

				if (implementation->_AudioProcessor->setupProcessing(setup) != Steinberg::kResultOk)
				{
					ASSERT(false, "Could not setup processing on audio processor!");
					Terminate();
					return false;
				}
			}

			//Start processing the audio processor!
			if (implementation->_AudioProcessor->setProcessing(true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not start processing the audio processor!");
				Terminate();
				return false;
			}

			//We found the audio effect class!
			found_audio_effect_class = true;

			//We have now found the audio effect class, break. (:
			break;
		}
	}

	//If we didn't find the audio effect class, ensure we clean up properly.
	if (!found_audio_effect_class)
	{
		Terminate();
		return false;
	}

	else
	{
		//This plugin host is now initialized. (:
		_Initialized = true;

		return true;
	}
}

/*
*	Sets a parameter (normalized) with the given identifier. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::SetParameterNormalized(const HashString identifier, const float64 value) NOEXCEPT
{
	//Can't set parameters if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Find the parameter.
	const Parameter *const RESTRICT parameter{ FindParameter(identifier) };

	if (!parameter)
	{
		ASSERT(false, "Could not find parameter!");

		return false;
	}

	//Set the value.
	if (implementation->_EditController->setParamNormalized(parameter->_FormatIdentifier._uint32, value) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not set parameter value!");

		return false;
	}

	else
	{
		return true;
	}
}

/*
*	Sets a parameter (plain) with the given identifier. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::SetParameterPlain(const HashString identifier, const float64 value) NOEXCEPT
{
	//Can't set parameters if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Find the parameter.
	const Parameter *const RESTRICT parameter{ FindParameter(identifier) };

	if (!parameter)
	{
		ASSERT(false, "Could not find parameter!");

		return false;
	}

	//Convert the value to normalized.
	const Steinberg::Vst::ParamValue normalized_value{ implementation->_EditController->plainParamToNormalized(parameter->_FormatIdentifier._uint32, value) };

	//Set the value.
	if (implementation->_EditController->setParamNormalized(parameter->_FormatIdentifier._uint32, normalized_value) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not set parameter value!");

		return false;
	}

	else
	{
		return true;
	}
}

/*
*	Shows the UI. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::ShowUI() NOEXCEPT
{
	//Can't show UI if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//If the plug frame no longer has a frame window (either by a 'HideUI()' call or the user closing the window manually, hide the UI so we can show it again.
	if (!implementation->_PlugFrame.HasFrameWindow())
	{
		HideUI();
	}

	//If we already have a view, then the UI is already shown.
	if (implementation->_EditorView)
	{
		return true;
	}

	//Create the view.
	implementation->_EditorView = implementation->_EditController->createView(Steinberg::Vst::ViewType::kEditor);

	if (!implementation->_EditorView)
	{
		ASSERT(false, "Could not create editor view!");
		return false;
	}

	//Check if the platform is supported.
	if (implementation->_EditorView->isPlatformTypeSupported(Steinberg::kPlatformTypeHWND) != Steinberg::kResultOk)
	{
		ASSERT(false, "Plugin does not support this platform!");
		HideUI();
		return false;
	}

	//Set the plug frame.
	if (implementation->_EditorView->setFrame(&implementation->_PlugFrame) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not set frame on editor view!");
		HideUI();
		return false;
	}

	//Retrieve the view rect.
	Steinberg::ViewRect view_rect;

	if (implementation->_EditorView->getSize(&view_rect) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not receive view rect!");
		HideUI();
		return false;
	}

	//Sanity check the view rect.
	if (view_rect.getWidth() == 0 || view_rect.getHeight() == 0)
	{
		ASSERT(false, "Editor view rect is 0!");
		HideUI();
		return false;
	}

	//Create the frame window.
	implementation->_PlugFrame.CreateFrameWindow(_PluginName.Data(), view_rect);

	//Attach the view.
	if (implementation->_EditorView->attached(implementation->_PlugFrame.GetFrameWindow(), Steinberg::kPlatformTypeHWND) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not attach editor view!");
		HideUI();
		return false;
	}

	//We should now show the UI.
	return true;
}

/*
*	Hides the UI. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::HideUI() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//If we don't have a view, then the UI is already hidden.
	if (!implementation->_EditorView)
	{
		return true;
	}

	//Set reset the frame on the editor view.
	if (implementation->_EditorView->setFrame(nullptr) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not reset frame on editor view!");
	}

	//Remove the editor view.
	if (implementation->_EditorView->removed() != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not remove editor view!");
	}

	//Release the editor view.
	implementation->_EditorView->release();

	//Reset the editor view pointer.
	implementation->_EditorView = nullptr;

	//Destroy the frame window on the plug frame.
	implementation->_PlugFrame.DestroyFrameWindow();

	//TODO. (:
	return true;
}

/*
*	Returns the state. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::GetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Can't retrieve the state if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Set up the bit stream.
	VST3::BitStream bit_stream;

	bit_stream.SetStreamArchive(stream_archive);

	//Retrieve the component state.
	return implementation->_AudioEffectComponent->getState(&bit_stream) == Steinberg::kResultOk;
}

/*
*	Sets the state. Returns if it succeeded.
*/
NO_DISCARD bool VST3PluginHost::SetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Can't set the state if we're not initialized.
	if (!_Initialized)
	{
		return false;
	}

	//Set up the bit stream.
	VST3::BitStream bit_stream;

	bit_stream.SetStreamArchive(stream_archive);

	//Set the component state.
	return implementation->_AudioEffectComponent->setState(&bit_stream) == Steinberg::kResultOk;
}

/*
*	Terminates this VST3 plugin.
*/
void VST3PluginHost::Terminate() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Hide the UI.
	HideUI();

	//Disconnect the connection points and release them.
	if (implementation->_ComponentConnectionPoint && implementation->_ControllerConnectionPoint)
	{
		if (implementation->_ComponentConnectionPoint->disconnect(implementation->_ControllerConnectionPoint) != Steinberg::kResultOk)
		{
			ASSERT(false, "Could not disconnect component connection point from the controller connection point!");
		}

		if (implementation->_ControllerConnectionPoint->disconnect(implementation->_ComponentConnectionPoint) != Steinberg::kResultOk)
		{
			ASSERT(false, "Could not disconnect controller connection point from the component connection point!");
		}
	}

	//Release the component connection point.
	if (implementation->_ComponentConnectionPoint)
	{
		implementation->_ComponentConnectionPoint->release();
		implementation->_ComponentConnectionPoint = nullptr;
	}

	//Release the controller connection point.
	if (implementation->_ControllerConnectionPoint)
	{
		implementation->_ControllerConnectionPoint->release();
		implementation->_ControllerConnectionPoint = nullptr;
	}

	//Stop processing on the audio processor.
	if (implementation->_AudioProcessor)
	{
		if (implementation->_AudioProcessor->setProcessing(false) != Steinberg::kResultOk)
		{
			ASSERT(false, "Failed to stop processing on the audio processor!");
		}
	}

	//Deactivate the audio effect component.
	if (implementation->_AudioEffectComponent)
	{
		if (implementation->_AudioEffectComponent->setActive(false) != Steinberg::kResultOk)
		{
			ASSERT(false, "Failed to deactivate the audio effect component!");
		}
	}

	//Release the audio processor.
	if (implementation->_AudioProcessor)
	{
		implementation->_AudioProcessor->release();
		implementation->_AudioProcessor = nullptr;
	}

	//Terminate the edit controller.
	if (!TerminateEditController())
	{
		ASSERT(false, "Could not terminate edit controller!");
	}

	//Destroy the edit controller.
	if (!DestroyEditController())
	{
		ASSERT(false, "Could not destroy the edit controller!");
	}

	//Release the audio effect component.
	if (implementation->_AudioEffectComponent)
	{
		if (implementation->_AudioEffectComponent->terminate() != Steinberg::kResultOk)
		{
			ASSERT(false, "Failed to terminate the audio effect component!");
		}

		implementation->_AudioEffectComponent->release();
		implementation->_AudioEffectComponent = nullptr;
	}

	//Release the plugin factory.
	if (implementation->_PluginFactory)
	{
		implementation->_PluginFactory->release();
		implementation->_PluginFactory = nullptr;
	}

	//Unload the dynamic library.
	if (implementation->_DynamicLibrary.IsLoaded())
	{
		//Retrieve the exit module function.
		ExitModuleProc exit_module_function{ static_cast<ExitModuleProc>(implementation->_DynamicLibrary.GetSymbol("ExitDll")) };

		if (!exit_module_function)
		{
			exit_module_function = static_cast<ExitModuleProc>(implementation->_DynamicLibrary.GetSymbol("ModuleExit"));
		}

		if (exit_module_function && !exit_module_function())
		{
			ASSERT(false, "Could not exit the module!");
		}

		//Unload the dynamic library.
		implementation->_DynamicLibrary.Unload();
	}

	//This plugin host is no longer initialized.
	_Initialized = false;
}

/*
*	Callback for this audio effect to process the given buffer.
*/
void VST3PluginHost::Process
(
	const AudioProcessContext &context,
	const DynamicArray<DynamicArray<float32>> &inputs,
	DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
	const uint8 number_of_channels,
	const uint32 number_of_samples
) NOEXCEPT
{
	//Can't process if we're not initialized.
	if (!_Initialized)
	{
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
		}

		return;
	}

	//Do some sanity checking.
	ASSERT(number_of_channels == 2, "This assumes we have stereo in/stereo out!");

	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Fill up the input/output buffer.
	implementation->_InputBuffer.Resize<true>(number_of_channels);
	implementation->_OutputBuffer.Resize<true>(number_of_channels);

	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		implementation->_InputBuffer.At(channel_index).Resize<false>(number_of_samples);
		implementation->_OutputBuffer.At(channel_index).Resize<false>(number_of_samples);

		Memory::Copy(implementation->_InputBuffer.At(channel_index).Data(), inputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
		Memory::Copy(implementation->_OutputBuffer.At(channel_index).Data(), outputs->At(channel_index).Data(), number_of_samples * sizeof(float32));
	}

	//Set up the process data.
	Steinberg::Vst::ProcessData process_data{ };

	process_data.processMode = Steinberg::Vst::ProcessModes::kRealtime;
	process_data.symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample32;
	process_data.numSamples = static_cast<int32>(number_of_samples);
	process_data.numInputs = 1;
	process_data.numOutputs = 1;

	float32 *RESTRICT input_pointers[2]
	{
		implementation->_InputBuffer.At(0).Data(),
		implementation->_InputBuffer.At(1).Data()
	};

	Steinberg::Vst::AudioBusBuffers input_buffers{ };

	input_buffers.numChannels = 2;
	input_buffers.silenceFlags = 0;
	input_buffers.channelBuffers32 = input_pointers;

	process_data.inputs = &input_buffers;

	float32 *RESTRICT output_pointers[2]
	{
		implementation->_OutputBuffer.At(0).Data(),
		implementation->_OutputBuffer.At(1).Data()
	};

	Steinberg::Vst::AudioBusBuffers output_buffers{ };

	output_buffers.numChannels = 2;
	output_buffers.silenceFlags = 0;
	output_buffers.channelBuffers32 = output_pointers;

	process_data.outputs = &output_buffers;

	process_data.inputParameterChanges = nullptr;
	process_data.outputParameterChanges = nullptr;
	process_data.inputEvents = nullptr;
	process_data.outputEvents = nullptr;

	Steinberg::Vst::ProcessContext process_context{ };

	process_context.state = Steinberg::Vst::ProcessContext::StatesAndFlags::kPlaying
							| Steinberg::Vst::ProcessContext::StatesAndFlags::kTempoValid;
	process_context.sampleRate = static_cast<float64>(_SampleRate);
	process_context.projectTimeSamples = implementation->_ProjectTime;
	process_context.tempo = static_cast<float64>(_BeatsPerMinute);

	process_data.processContext = &process_context;

	//Process!
	if (implementation->_AudioProcessor->process(process_data) != Steinberg::kResultOk)
	{
		ASSERT(false, "Failed to process!");
	}

	//Copy the outputs.
	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		Memory::Copy(outputs->At(channel_index).Data(), implementation->_OutputBuffer.At(channel_index).Data(), number_of_samples * sizeof(float32));
	}

	//Update the project time.
	implementation->_ProjectTime += static_cast<int64>(number_of_samples);
}

/*
*	Returns the implementation.
*/
NO_DISCARD VST3PluginHostImplementation *const RESTRICT VST3PluginHost::Implementation() NOEXCEPT
{
	return _Implementation.Get<VST3PluginHostImplementation>();
}

/*
*	Creates the edit controller.
*/
NO_DISCARD bool VST3PluginHost::CreateEditController() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//First, check if the component itsels is a controller.
	if (implementation->_AudioEffectComponent->queryInterface(Steinberg::Vst::IEditController_iid, reinterpret_cast<void**>(&implementation->_EditController)) != Steinberg::kResultOk)
	{
		//Set the edit controller to 'nullptr' in case the plugin did something weird.
		implementation->_EditController = nullptr;
	}

	//Second, check if the controller exposes a separate controller class that can be created.
	if (!implementation->_EditController)
	{
		//Check if we can retrieve the controller class ID.
		Steinberg::TUID controller_class_id;

		if (implementation->_AudioEffectComponent->getControllerClassId(controller_class_id) == Steinberg::kResultOk)
		{
			//Create the edit controller.
			if (implementation->_PluginFactory->createInstance(controller_class_id, Steinberg::Vst::IEditController_iid, reinterpret_cast<void**>(&implementation->_EditController)) != Steinberg::kResultOk)
			{
				//Set the edit controller to 'nullptr' in case the plugin did something weird.
				implementation->_EditController = nullptr;
			}
		}
	}

	//Set the component handler on the edit controller.
	if (implementation->_EditController)
	{
		if (implementation->_EditController->setComponentHandler(&implementation->_ComponentHandler) != Steinberg::kResultOk)
		{
			ASSERT(false, "Could not set component handler!");
		}
	}

	//Set the edit controller on the component handler.
	if (implementation->_EditController)
	{
		implementation->_ComponentHandler.SetEditController(implementation->_EditController);
	}

	//Return if the edit controller was created.
	return implementation->_EditController != nullptr;
}

/*
*	Initializes the edit controller.
*/
NO_DISCARD bool VST3PluginHost::InitializeEditController() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//If the audio effect component _is_ the edit controller, then it has already been initialized, so no need to do it here.
	if (!VST3::Utilities::Compareobjects(implementation->_AudioEffectComponent, implementation->_EditController))
	{
		//Try to initialize the edit controller.
		if (implementation->_EditController->initialize(&implementation->_HostApplication) != Steinberg::kResultOk)
		{
			return false;
		}
	}

	return true;
}

/*
*	Terminates the edit controller.
*/
NO_DISCARD bool VST3PluginHost::TerminateEditController() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//If the audio effect component _is_ the edit controller, then it will be terminate, so no need to do it here.
	if (!VST3::Utilities::Compareobjects(implementation->_AudioEffectComponent, implementation->_EditController))
	{
		//Try to initialize the edit controller.
		if (implementation->_EditController->terminate() != Steinberg::kResultOk)
		{
			return false;
		}
	}

	return true;
}

/*
*	Destroys the edit controller.
*/
NO_DISCARD bool VST3PluginHost::DestroyEditController() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Destroy the edit controller.
	if (implementation->_EditController)
	{
		implementation->_EditController->release();
		implementation->_EditController = nullptr;
	}

	//There's nothing that can fail here really, so just return true.
	return true;
}