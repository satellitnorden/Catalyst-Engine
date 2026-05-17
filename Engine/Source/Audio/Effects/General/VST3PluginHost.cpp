//Header file.
#include <Audio/Effects/General/VST3PluginHost.h>

//Core.
#include <Core/General/DynamicLibrary.h>
#include <Core/Utilities/StringUtilities.h>

//Audio.
#include <Audio/VST3/BitStream.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <VST3/pluginterfaces/base/ipluginbase.h>
#include <VST3/pluginterfaces/vst/ivstaudioprocessor.h>
#include <VST3/pluginterfaces/vst/ivsteditcontroller.h>
#include <VST3/pluginterfaces/vst/ivsthostapplication.h>

//STD.
#include <atomic>

/*
*	Host application class definition.
*/
class HostApplication final : public Steinberg::Vst::IHostApplication
{

public:

	/*
	*	Queries an instance.
	*/
	FORCE_INLINE Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void **obj) override
	{
		//Check that we have valid arguments.
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		//Reset the object.
		*obj = nullptr;

		//Are they asking for an unknown?
		if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::FUnknown_iid))
		{
			*obj = static_cast<Steinberg::FUnknown*>(this);
		}

		//Are they asking for a host application?
		else if (Steinberg::FUnknownPrivate::iidEqual(_iid, Steinberg::Vst::IHostApplication_iid))
		{
			*obj = static_cast<Steinberg::Vst::IHostApplication*>(this);
		}

		//Add a reference if we set the object.
		if (*obj)
		{
			addRef();
			return Steinberg::kResultOk;
		}

		//Otherwise, return that we don't have this interface.
		else
		{
			return Steinberg::kNoInterface;
		}
	}

	/*
	*	Queries an instance.
	*/
	FORCE_INLINE Steinberg::uint32 addRef() override
	{
		return ++_ReferenceCount;
	}

	/*
	*	Queries an instance.
	*/
	FORCE_INLINE Steinberg::uint32 release() override
	{
		const uint32 previous_reference_count{ _ReferenceCount.fetch_sub(1) };

		ASSERT(previous_reference_count > 0, "Double release detected!");

		return previous_reference_count > 0 ? previous_reference_count - 1 : 0;
	}

	/*
	*	Returns the name.
	*/
	FORCE_INLINE Steinberg::tresult getName(Steinberg::Vst::String128 name) override
	{
		constexpr const char *const RESTRICT NAME{ "Catalyst Engine" };
		constexpr uint64 NAME_LENGTH{ StringUtilities::StringLength(NAME) };

		for (uint64 i{ 0 }; i < NAME_LENGTH; ++i)
		{
			name[i] = static_cast<Steinberg::Vst::TChar>(NAME[i]);
		}

		name[NAME_LENGTH] = static_cast<Steinberg::Vst::TChar>('\0');

		return Steinberg::kResultOk;
	}

	/*
	*	Creates an instance.
	*/
	FORCE_INLINE Steinberg::tresult createInstance(Steinberg::TUID cid, Steinberg::TUID _iid, void **obj) override
	{
		if (!obj)
		{
			return Steinberg::kInvalidArgument;
		}

		*obj = nullptr;

		return Steinberg::kNoInterface;
	}

private:

	//The reference count.
	std::atomic<uint32> _ReferenceCount{ 1 };

};

/*
*	VST3 plugin host implementation class definition.
*/
class VST3PluginHostImplementation final
{

public:

	//The dynamic library.
	DynamicLibrary _DynamicLibrary;

	//The host application.
	HostApplication _HostApplication;

	//The audio effect component.
	Steinberg::Vst::IComponent *RESTRICT _AudioEffectComponent{ nullptr };

	//The audio processor component.
	Steinberg::Vst::IAudioProcessor *RESTRICT _AudioProcessor{ nullptr };

	//The audio edit controller.
	Steinberg::Vst::IEditController *RESTRICT _EditController{ nullptr };

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The output buffer.
	DynamicArray<DynamicArray<float32>> _OutputBuffer;

};

constexpr size_t _SIZE = sizeof(VST3PluginHostImplementation);

static_assert(sizeof(VST3PluginHostImplementation) <= VST3PluginHost::IMPLEMENTATION_SIZE, "Increase implementation size!");

/*
*	Returns the result string.
*/
FORCE_INLINE NO_DISCARD const char* const RESTRICT ResultString(const Steinberg::tresult result) NOEXCEPT
{
	switch (result)
	{
		case Steinberg::kNoInterface:
		{
			return "kNoInterface";
		}

		case Steinberg::kResultOk:
		{
			return "kResultOk";
		}

		case Steinberg::kResultFalse:
		{
			return "kResultFalse";
		}

		case Steinberg::kInvalidArgument:
		{
			return "kInvalidArgument";
		}

		case Steinberg::kNotImplemented:
		{
			return "kNotImplemented";
		}

		case Steinberg::kInternalError:
		{
			return "kInternalError";
		}

		case Steinberg::kNotInitialized:
		{
			return "kNotInitialized";
		}

		case Steinberg::kOutOfMemory:
		{
			return "kOutOfMemory";
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			return "UNKNOWN";
		}
	}
}

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
*	Initializes this VST3 plugin.
*/
void VST3PluginHost::Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT
{
	//Define constants.
	constexpr auto ConvertString128
	{
		[](const Steinberg::Vst::String128 &input_string)
		{
			StaticString<128> output_string;

			{
				uint32 current_index{ 0 };

				while (input_string[current_index] != '\0')
				{
					output_string[current_index] = static_cast<char>(input_string[current_index]);
					++current_index;
				}

				output_string[current_index] = '\0';
			}

			return output_string;
		}
	};

	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Keep a result variable around.
	Steinberg::tresult result{ Steinberg::kResultOk };

	//Load the dynamic library.
	if (!implementation->_DynamicLibrary.Load(plugin_file_path))
	{
		ASSERT(false, "Couldn't load %s!", plugin_file_path);
		return;
	}

	//Retrieve the "GetPluginFactory()" function.
	GetFactoryProc get_plugin_factory_function{ static_cast<GetFactoryProc>(implementation->_DynamicLibrary.GetSymbol("GetPluginFactory")) };

	if (!get_plugin_factory_function)
	{
		ASSERT(false, "Could not retrieve \"GetPluginFactory()\" function!");
		return;
	}

	//Retrieve the plugin factory.
	Steinberg::IPluginFactory *const RESTRICT plugin_factory{ get_plugin_factory_function() };

	if (!plugin_factory)
	{
		ASSERT(false, "Could not retrieve plugin factory!");
		return;
	}

	//Retrieve the factory info.
	Steinberg::PFactoryInfo factory_info;

	if (plugin_factory->getFactoryInfo(&factory_info) != Steinberg::kResultOk)
	{
		ASSERT(false, "Could not retrieve factory info!");
		return;
	}

	//Retrieve the number of classes.
	const int32 number_of_classes{ plugin_factory->countClasses() };

	//Iterate over the classes.
	for (int32 class_index{ 0 }; class_index < number_of_classes; ++class_index)
	{
		//Retrieve the class info.
		Steinberg::PClassInfo class_info;

		if (plugin_factory->getClassInfo(class_index, &class_info) != Steinberg::kResultOk)
		{
			ASSERT(false, "Could not retrieve class info!");
			return;
		}

		//Is this the audio effect class?
		if (StringUtilities::IsEqual(class_info.category, kVstAudioEffectClass))
		{
			result = plugin_factory->createInstance
			(
				class_info.cid,
				Steinberg::Vst::IComponent_iid,
				reinterpret_cast<void**>(&implementation->_AudioEffectComponent)
			);

			if (result != Steinberg::kResultOk)
			{
				ASSERT(false, "Couldn't create audio effect component!");
				return;
			}

			//Initialize the audio effect component.
			if (implementation->_AudioEffectComponent->initialize(&implementation->_HostApplication) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not initialize audio effect component!");
				return;
			}

			//Create the component connection point.
			Steinberg::Vst::IConnectionPoint *RESTRICT component_connection_point{ nullptr };

			if (implementation->_AudioEffectComponent->queryInterface(Steinberg::Vst::IConnectionPoint_iid, reinterpret_cast<void**>(&component_connection_point)) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not create component connection point!");
				return;
			}

			//Retrieve the controller class ID.
			Steinberg::TUID controller_class_id;

			if (implementation->_AudioEffectComponent->getControllerClassId(controller_class_id) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not retrieve controller class ID from audio effect component!");
				return;
			}

			//Create the edit controller.
			if (plugin_factory->createInstance(controller_class_id, Steinberg::Vst::IEditController_iid, reinterpret_cast<void**>(&implementation->_EditController)) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not create edit controller instance!");
				return;
			}

			//Initialize the edit controller.
			if (implementation->_EditController->initialize(&implementation->_HostApplication) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not initialize edit controller!");
				return;
			}

			//Create the controller connection point.
			Steinberg::Vst::IConnectionPoint *RESTRICT controller_connection_point{ nullptr };

			if (implementation->_EditController->queryInterface(Steinberg::Vst::IConnectionPoint_iid, reinterpret_cast<void**>(&controller_connection_point)) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not create controller connection point!");
				return;
			}

			//Connect the component and the controller.
			if (component_connection_point->connect(controller_connection_point) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not connect controller to component!");
				return;
			}
			
			if (controller_connection_point->connect(component_connection_point) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not connect component to controller!");
				return;
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
						return;
					}

					_Parameters.Emplace();
					Parameter &new_parameter{ _Parameters.Back() };

					new_parameter._Identifier = parameter_info.id;
					new_parameter._Name = ConvertString128(parameter_info.title);
					new_parameter._DefaultNormalizedValue = parameter_info.defaultNormalizedValue;
				}
			}

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

					if (bus_info.channelCount == 2)
					{
						has_stereo_input = true;

						break;
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

					if (bus_info.channelCount == 2)
					{
						has_stereo_output = true;

						break;
					}
				}
			}

			if (!has_stereo_input || !has_stereo_output)
			{
				ASSERT(false, "We assume stereo input and output, handle this!");
				return;
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
				return;
			}

			//Set the bus arrangements.
			{
				Steinberg::Vst::SpeakerArrangement INPUT_ARRANGEMENT{ Steinberg::Vst::SpeakerArr::kStereo };
				Steinberg::Vst::SpeakerArrangement OUTPUT_ARRANGEMENT{ Steinberg::Vst::SpeakerArr::kStereo };

				if (implementation->_AudioProcessor->setBusArrangements(&INPUT_ARRANGEMENT, 1, &OUTPUT_ARRANGEMENT, 1) != Steinberg::kResultOk)
				{
					ASSERT(false, "Could not set bus arrangements on the audio processor!");
					return;
				}
			}

			//Activate the buses.
			if (implementation->_AudioEffectComponent->activateBus(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kInput, 0, true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not activate input bus on audio effect component!");
				return;
			}

			if (implementation->_AudioEffectComponent->activateBus(Steinberg::Vst::MediaTypes::kAudio, Steinberg::Vst::BusDirections::kOutput, 0, true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not activate output bus on audio effect component!");
				return;
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
					return;
				}
			}

			//Activate the audio effect component.
			if (implementation->_AudioEffectComponent->setActive(true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not activate audio effect component!");
				return;
			}

			//Start processing the audio processor!
			if (implementation->_AudioProcessor->setProcessing(true) != Steinberg::kResultOk)
			{
				ASSERT(false, "Could not start processing the audio processor!");
				return;
			}

			//Release the connection points.
			component_connection_point->release();
			controller_connection_point->release();

			//We have not found the audio effect class, break. (:
			break;
		}
	}
}

/*
*	Terminates this VST3 plugin.
*/
void VST3PluginHost::Terminate() NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

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

	//Release the edit controller.
	if (implementation->_EditController)
	{
		implementation->_EditController->terminate();
		implementation->_EditController->release();
		implementation->_EditController = nullptr;
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

	//Unload the dynamic library.
	if (implementation->_DynamicLibrary.IsLoaded())
	{
		implementation->_DynamicLibrary.Unload();
	}
}

/*
*	Sets a parameter with the given identifier to the given value.
*/
void VST3PluginHost::SetParameter(const char *const RESTRICT name, const float64 value) NOEXCEPT
{
	//Cache the implementation.
	VST3PluginHostImplementation *const RESTRICT implementation{ Implementation() };

	//Find the parameter.
	for (const Parameter &parameter : _Parameters)
	{
		if (parameter._Name == name)
		{
			//Set the value.
			implementation->_EditController->setParamNormalized(parameter._Identifier, value);

			return;
		}
	}

	ASSERT(false, "Could not find parameter with name: '%s'", name);
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
	Steinberg::Vst::ProcessData process_data;

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

	Steinberg::Vst::AudioBusBuffers input_buffers;

	input_buffers.numChannels = 2;
	input_buffers.silenceFlags = 0;
	input_buffers.channelBuffers32 = input_pointers;

	process_data.inputs = &input_buffers;

	float32 *RESTRICT output_pointers[2]
	{
		implementation->_OutputBuffer.At(0).Data(),
		implementation->_OutputBuffer.At(1).Data()
	};

	Steinberg::Vst::AudioBusBuffers output_buffers;

	output_buffers.numChannels = 2;
	output_buffers.silenceFlags = 0;
	output_buffers.channelBuffers32 = output_pointers;

	process_data.outputs = &output_buffers;

	process_data.inputParameterChanges = nullptr;
	process_data.outputParameterChanges = nullptr;
	process_data.inputEvents = nullptr;
	process_data.outputEvents = nullptr;
	process_data.processContext = nullptr;

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
}

/*
*	Returns the implementation.
*/
NO_DISCARD VST3PluginHostImplementation *const RESTRICT VST3PluginHost::Implementation() NOEXCEPT
{
	return _Implementation.Get<VST3PluginHostImplementation>();
}