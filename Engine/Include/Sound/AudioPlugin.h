#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/ArrayProxy.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StreamArchive.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox2D.h>

//Sound.
#include <Sound/AudioProcessContext.h>
#include <Sound/MIDIMessage.h>

//User interface.
#include <UserInterface/UserInterfaceCore.h>

/*
*	This is a base class for audio plugins, meant to represent some generic processing unit that can be reused in different contexts.
*/
class AudioPlugin
{

public:

	/*
	*	Initialize parameters class definition.
	*/
	class InitializeParameters final
	{

	public:

		//The directory that this plugin operates in.
		DynamicString _Directory;

		//The sample rate.
		float32 _SampleRate;

		//The beats per minute.
		float32 _BeatsPerMinute;

	};

	/*
	*	Parameter class definition.
	*/
	class Parameter final
	{

	public:

		//Enumeration covering all formats.
		enum class Format : uint8
		{
			NATIVE,
			BOOL,
			ENUMERATION,
			FREQUENCY,
			GAIN,
			PERCENT,
			MILLISECONDS,
			UNSIGNED_INTEGER_32
		};

		//The name.
		const char *RESTRICT _Name;

		//The display name.
		const char *RESTRICT _DisplayName{ nullptr };

		//The tooltip.
		const char *RESTRICT _Tooltip{ "" };

		//The identifier.
		HashString _Identifier;

		//The index.
		uint64 _Index;

		//The data.
		void *RESTRICT _Data;

		//The minimum.
		float32 _Minimum;

		//The maximum.
		float32 _Maximum;

		//The step.
		float64 _Step;

		//The format.
		Format _Format;

		//The enumeration names. Only used if _Format is Format::ENUMERATION.
		ArrayProxy<const char*> _EnumerationNames;

		//The enumeration tooltips. Only used if _Format is Format::ENUMERATION.
		ArrayProxy<const char*> _EnumerationTooltips;

	};

	/*
	*	Preset class definition.
	*/
	class Preset final
	{

	public:

		/*
		*	Parameter class definition.
		*/
		class Parameter final
		{

		public:

			//The identifier.
			HashString _Identifier;

			//The value.
			union
			{
				float32 _Value_float32;
				bool _Value_bool;
				uint32 _Value_uint32;
			};

		};

		//The name.
		const char *RESTRICT _Name;

		//The parameters.
		DynamicArray<Parameter> _Parameters;

		/*
		*	Adds the parameter with the given identifier with the given value.
		*/
		FORCE_INLINE void Add(const HashString identifier, const float32 value) NOEXCEPT
		{
			//Check if there's already a parameter with this identifer.
			for (Parameter &parameter : _Parameters)
			{
				if (parameter._Identifier == identifier)
				{
					parameter._Value_float32 = value;

					return;
				}
			}

			//Otherwise, add it!
			_Parameters.Emplace();
			Parameter &parameter{ _Parameters.Back() };

			parameter._Identifier = identifier;
			parameter._Value_float32 = value;
		}

		FORCE_INLINE void Add(const HashString identifier, const bool value) NOEXCEPT
		{
			//Check if there's already a parameter with this identifer.
			for (Parameter &parameter : _Parameters)
			{
				if (parameter._Identifier == identifier)
				{
					parameter._Value_bool = value;

					return;
				}
			}

			//Otherwise, add it!
			_Parameters.Emplace();
			Parameter &parameter{ _Parameters.Back() };

			parameter._Identifier = identifier;
			parameter._Value_bool = value;
		}

		FORCE_INLINE void Add(const HashString identifier, const uint32 value) NOEXCEPT
		{
			//Check if there's already a parameter with this identifer.
			for (Parameter &parameter : _Parameters)
			{
				if (parameter._Identifier == identifier)
				{
					parameter._Value_uint32 = value;

					return;
				}
			}

			//Otherwise, add it!
			_Parameters.Emplace();
			Parameter &parameter{ _Parameters.Back() };

			parameter._Identifier = identifier;
			parameter._Value_uint32 = value;
		}

	};

	/*
	*	Control class definition.
	*/
	class Control final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			BUTTON,
			DROP_DOWN,
			FADER,
			TEXT,
			KNOB,
			CHECKBOX,
			SELECTOR,
			SWITCH
		};

		//The bounding box.
		AxisAlignedBoundingBox2D _BoundingBox;

		//The type.
		Type _Type;

		//The parameter.
		HashString _Parameter;

		//The update function.
		void(*_UpdateFunction)(AudioPlugin *RESTRICT plugin, Control *RESTRICT control) { nullptr };

		//Denotes whether or not this control is visible.
		bool _Visible{ true };

		//The user data.
		void *RESTRICT _UserData;

		struct
		{
			//Denotes whether or not this button is enabled.
			bool _Enabled{ true };

			//The text.
			const char *RESTRICT _Text;

			//The callback.
			void(*_Callback)(AudioPlugin *RESTRICT plugin, Control *RESTRICT control) { nullptr };
		} _ButtonData;

		struct
		{
			//The text.
			const char *RESTRICT _Text;

			//The drop down items.
			ArrayProxy<const char *> _DropDownItems;

			//The callback.
			void(*_Callback)(AudioPlugin *RESTRICT plugin, Control *RESTRICT control, const uint32 chosen_item_index) { nullptr };
		} _DropDownData;

		struct
		{
			//The text.
			const char *RESTRICT _Text;

			//The size.
			float32 _Size{ 16.0f };

			//The horizontal alignment.
			TextHorizontalAlignment _HorizontalAlignment{ TextHorizontalAlignment::CENTER };
		} _TextData;

	};

	/*
	*	Control tab class definition.
	*/
	class ControlTab final
	{

	public:

		//The name.
		const char *RESTRICT _Name;

		//The controls.
		DynamicArray<Control> _Controls;

	};

	/*
	*	Control layout class definition.
	*/
	class ControlLayout final
	{

	public:

		//The controls.
		DynamicArray<Control> _Controls;

		//The tabs.
		DynamicArray<ControlTab> _Tabs;

	};

	/*
	*	Piano roll information class definition.
	*/
	class PianoRollInformation final
	{

	public:

		/*
		*	Key information class definition.
		*/
		class KeyInformation final
		{

		public:

			//The color.
			Vector3<float32> _Color{ -1.0f, -1.0f, -1.0f };

			//The tooltip.
			const char *RESTRICT _Tooltip{ "" };
		
		};

		//The key information.
		StaticArray<KeyInformation, 127> _KeyInformation;

	};

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~AudioPlugin() NOEXCEPT
	{

	}

	/*
	*	Returns if this audio plugin wants a piano roll.
	*/
	FORCE_INLINE virtual NO_DISCARD bool WantsPianoRoll() const NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns the piano roll information.
	*/
	FORCE_INLINE virtual void GetPianoRollInformation(PianoRollInformation *const RESTRICT information) NOEXCEPT
	{

	}

	/*
	*	Returns the latency, in seconds.
	*/
	FORCE_INLINE virtual NO_DISCARD float32 GetLatency() NOEXCEPT
	{
		return 0.0f;
	}

	/*
	*	Returns the parameters.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Parameter> &GetParameters() const NOEXCEPT
	{
		return _Parameters;
	}

	/*
	*	Returns the presets.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<Preset> &GetPresets() const NOEXCEPT
	{
		return _Presets;
	}

	/*
	*	Returns the control layout.
	*/
	FORCE_INLINE NO_DISCARD ControlLayout *const RESTRICT GetControlLayout() NOEXCEPT
	{
		return &_ControlLayout;
	}

	/*
	*	Serializes state.
	*/
	FORCE_INLINE virtual void SerializeState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT
	{

	}

	/*
	*	Un-serializes state.
	*/
	FORCE_INLINE virtual void UnSerializeState(StreamArchive *const RESTRICT stream_archive, uint64 *const RESTRICT stream_archive_position) NOEXCEPT
	{

	}

	/*
	*	Initializes this audio plugin.
	*/
	FORCE_INLINE virtual void Initialize(const InitializeParameters &parameters) NOEXCEPT
	{
		//Store the initialize parameters.
		_InitializeParameters = parameters;
	}

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const float32 value) NOEXCEPT
	{
		if (_SampleRate != value)
		{
			_SampleRate = value;
			OnSampleRateChanged();
		}
	}

	/*
	*	Sets the beats per minute.
	*/
	FORCE_INLINE void SetBeatsPerMinute(const float32 value) NOEXCEPT
	{
		if (_BeatsPerMinute != value)
		{
			_BeatsPerMinute = value;
			OnBeatsPerMinuteChanged();
		}
	}

	/*
	*	Callback for this audio plugin to process the given buffer.
	*/
	FORCE_INLINE virtual void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint32 number_of_samples,
		const uint8 number_of_channels
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
	*	Callback for when this plugin receives MIDI messages.
	*/
	FORCE_INLINE virtual void OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT
	{

	}

	/*
	*	Callback for when a parameter is changed.
	*/
	FORCE_INLINE virtual void OnParameterChanged(const Parameter &parameter) NOEXCEPT
	{

	}

	/*
	*	Processes output MIDI messages with the given function and clears it afterwards.
	*/
	FORCE_INLINE void ProcessOutputMIDIMessages(void(*process_function)(const MIDIMessage &output_midi_message, void *const RESTRICT user_data), void *const RESTRICT user_data) NOEXCEPT
	{
		for (const MIDIMessage &output_midi_message : _OutputMIDIMessages)
		{
			process_function(output_midi_message, user_data);
		}

		_OutputMIDIMessages.Clear();
	}

protected:

	//The initialize parameters.
	InitializeParameters _InitializeParameters;

	//The parameters.
	DynamicArray<Parameter> _Parameters;

	//The presets.
	DynamicArray<Preset> _Presets;

	//The control layout.
	ControlLayout _ControlLayout;

	//The sample rate.
	float32 _SampleRate{ 48'000.0f };

	//The beats per minute.
	float32 _BeatsPerMinute{ 120.0f };

	//The output MIDI messages. These can be set anytime and the host will pick it up.
	DynamicArray<MIDIMessage> _OutputMIDIMessages;

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE virtual void OnSampleRateChanged() NOEXCEPT
	{

	}

	/*
	*	Callback for when the beats per minute changed.
	*/
	FORCE_INLINE virtual void OnBeatsPerMinuteChanged() NOEXCEPT
	{

	}

	/*
	*	Returns the preset with the given name.
	*/
	FORCE_INLINE NO_DISCARD Preset *const RESTRICT FindPreset(const char *const RESTRICT name) NOEXCEPT
	{
		for (Preset &preset : _Presets)
		{
			if (StringUtilities::IsEqual(preset._Name, name))
			{
				return &preset;
			}
		}

		ASSERT(false, "Couldn't preset with name %s!", name);

		return nullptr;
	}

};