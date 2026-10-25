#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/HashString.h>
#include <Core/General/StaticString.h>

//Audio.
#include <Audio/Core/Audio.h>
#include <Audio/Core/AudioProcessContext.h>
#include <Audio/MIDI/MIDIMessage.h>

//Concurrency.
#include <Concurrency/Atomic.h>
#include <Concurrency/ScopedLock.h>
#include <Concurrency/Spinlock.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//STD.
#include <cstdio>

//Forward declarations.
class UISystem;

namespace Audio
{

	/*
	*	Base class for an audio plugin.
	*	Exposes functionality for building audio plugins that can be loaded by a DAW, such as .clap or .vst3 plugins.
	*/
	class Plugin
	{

		/////////////
		// GENERAL //
		/////////////

	public:

		//Enumeration covering all main categories.
		enum class MainCategory : uint8
		{
			NONE,

			INSTRUMENT
		};

		//Enumeration covering all sub categories.
		enum class SubCategory : uint8
		{
			NONE,

			SYNTHESIZER
		};

		/*
		*	Initialize context class definition.
		*/
		class InitializeContext final
		{

		public:

			//The UI system.
			UISystem *RESTRICT _UISystem;

		};

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Plugin() NOEXCEPT;

		/*
		*	Returns the main category.
		*/
		FORCE_INLINE virtual NO_DISCARD MainCategory GetMainCategory() const NOEXCEPT;

		/*
		*	Returns the sub category.
		*/
		FORCE_INLINE virtual NO_DISCARD SubCategory GetSubCategory() const NOEXCEPT;

		/*
		*	Returns the resolution.
		*/
		FORCE_INLINE virtual NO_DISCARD Resolution GetResolution() const NOEXCEPT;

		/*
		*	Calls the 'Initialize()' function.
		*/
		FORCE_INLINE void CallInitialize(const InitializeContext &context) NOEXCEPT;

		/*
		*	Calls the 'Process()' function.
		*/
		FORCE_INLINE void CallProcess
		(
			const AudioProcessContext &context,
			const DynamicArray<DynamicArray<float32>> &inputs,
			DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
			const uint8 number_of_channels,
			const uint32 number_of_samples
		) NOEXCEPT;

	private:

		/*
		*	Request class definition.
		*/
		class Request final
		{

		public:

			//Enumeration covering all types.
			enum class Type : uint8
			{
				NONE,

				SET_PARAMETER,
				ADD_MIDI_MESSAGE
			};

			//The type.
			Type _Type;

			//The data
			union
			{
				//The set parameter value data.
				struct
				{
					//The index.
					uint64 _Index;

					//The value.
					float32 _Value;
				} _SetParameterValueData;

				//The add MIDI message data.
				struct
				{
					//The message.
					MIDIMessage _Message;

					//The sample offset.
					uint32 _SampleOffset;
				} _AddMIDIMessageData;
			};

		};

		//The requests lock.
		Spinlock _RequestsLock;

		//The requests.
		DynamicArray<Request> _Requests;

		//The inputs.
		DynamicArray<DynamicArray<float32>> _Inputs;

		//The outputs.
		DynamicArray<DynamicArray<float32>> _Outputs;

		/*
		*	Initializes this plugin.
		*/
		FORCE_INLINE virtual void Initialize(const InitializeContext &context) NOEXCEPT;

		/*
		*	Post-initializes this plugin.
		*/
		FORCE_INLINE virtual void PostInitialize(const InitializeContext &context) NOEXCEPT;

		/*
		*	Callback for this plugin to process the given buffer.
		*/
		FORCE_INLINE virtual void Process
		(
			const AudioProcessContext &context,
			const DynamicArray<DynamicArray<float32>> &inputs,
			DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
			const uint8 number_of_channels,
			const uint32 number_of_samples
		) NOEXCEPT;

		/*
		*	Callback for this plugin when it receives a MIDI message.
		*/
		FORCE_INLINE virtual void OnMIDIMessage(const MIDIMessage &message) NOEXCEPT;

		/*
		*	Processes requests.
		*/
		FORCE_INLINE void ProcessRequests() NOEXCEPT;

		////////////////
		// PARAMETERS //
		////////////////

	public:

		/*
		*	Returns the number of parameters.
		*/
		FORCE_INLINE NO_DISCARD uint64 GetNumberOfParameters() const NOEXCEPT;

		/*
		*	Finds the parameter index with the given name.
		*	Returns UINT64_MAXIMUM if it failed.
		*/
		FORCE_INLINE NO_DISCARD uint64 GetParameterIndex(const char *const RESTRICT name) const NOEXCEPT;

		/*
		*	Returns the parameter name at the given index.
		*/
		FORCE_INLINE NO_DISCARD const char *const RESTRICT GetParameterName(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter display name at the given index.
		*/
		FORCE_INLINE NO_DISCARD const char *const RESTRICT GetParameterDisplayName(const uint64 index) const NOEXCEPT;

		/*
		*	Returns if the parameter is stepped.
		*/
		FORCE_INLINE NO_DISCARD bool IsParameterStepped(const uint64 index) const NOEXCEPT;

		/*
		*	Returns if the parameter is an enumeration.
		*/
		FORCE_INLINE NO_DISCARD bool IsParameterEnumeration(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter minimum at the given index.
		*/
		FORCE_INLINE NO_DISCARD float32 GetParameterMinimum(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter maximum at the given index.
		*/
		FORCE_INLINE NO_DISCARD float32 GetParameterMaximum(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter default at the given index.
		*/
		FORCE_INLINE NO_DISCARD float32 GetParameterDefault(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter step normalized at the given index.
		*/
		FORCE_INLINE NO_DISCARD float32 GetParameterStepNormalized(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter current plain at the given index.
		*/
		FORCE_INLINE NO_DISCARD float32 GetParameterCurrentPlain(const uint64 index) const NOEXCEPT;

		/*
		*	Returns the parameter current normalized at the given index.
		*/
		FORCE_INLINE NO_DISCARD float32 GetParameterCurrentNormalized(const uint64 index) const NOEXCEPT;

		/*
		*	Sets a parameter plain.
		*/
		FORCE_INLINE void SetParameterPlain(const uint64 index, const float32 value) NOEXCEPT;

		/*
		*	Sets a parameter normalized.
		*/
		FORCE_INLINE void SetParameterNormalized(const uint64 index, const float32 value) NOEXCEPT;

		/*
		*	Prints the value string for the parameter with the given index.
		*/
		FORCE_INLINE NO_DISCARD bool PrintParameterValueString(const uint64 index, char *buffer, const uint64 buffer_capacity, const float32 value) const NOEXCEPT;

	protected:

		/*
		*	Parameter class definition.
		*/
		class Parameter final
		{

		public:

			//Enumeration covering all types.
			enum class Type : uint8
			{
				NONE,

				BOOL,
				ENUMERATION,
				GAIN,
				TIME
			};

			//The name.
			StaticString<64> _Name;

			//The display name.
			StaticString<64> _DisplayName;

			//The data.
			void *RESTRICT _Data;

			//The type.
			Type _Type;

			//The minimum.
			float32 _Minimum;

			//The maximum.
			float32 _Maximum;

			//The enumeration names. Only used when type is ENUMERATION.
			DynamicArray<StaticString<64>> _EnumerationNames;

		};

		//The parameters.
		DynamicArray<Parameter> _Parameters;

	private:

		/*
		*	Parameter state class definition.
		*/
		class ParameterState final
		{

		public:

			//The identifier.
			HashString _Identifier;

			//The default.
			float32 _Default;

			/*
			*	The current.
			*	This is used mainly for the main thread,
			*	It is assumed that the audio thread will access the current value through the actual stored variable referenced y the '_Data' variable instead.
			*/
			Atomic<float32> _Current;

		};

		//The parameter state.
		DynamicArray<ParameterState> _ParameterState;

		/*
		*	Sets a parameter internally.
		*/
		FORCE_INLINE void SetParameterInternal(const uint64 index, const float32 value) NOEXCEPT;

		/*
		*	Converts parameter data to a plain value.
		*/
		FORCE_INLINE NO_DISCARD float32 ParameterDataToPlain(const void *const RESTRICT data, const Parameter::Type type);

		//////////
		// MIDI //
		//////////

	public:

		/*
		*	MIDI note state class definition.
		*/
		class MIDINoteState final
		{

		public:

			//Denotes whether or not this note is pressed.
			Atomic<bool> _Pressed{ false };

		};

		/*
		*	Adds a MIDI message, optionally with a list of sample offsets.
		*/
		FORCE_INLINE void AddMIDIMessage
		(
			const MIDIMessage &messages,
			const uint32 *const RESTRICT sample_offset = nullptr
		) NOEXCEPT;

		/*
		*	Retrieves the note state for the given MIDI note.
		*/
		FORCE_INLINE NO_DISCARD const MIDINoteState &GetMIDINoteState(const uint8 note) const NOEXCEPT;

	private:

		/*
		*	Queued MIDI message class definition.
		*/
		class QueuedMIDIMessage final
		{

		public:

			//The message.
			MIDIMessage _Message;

			//The sample offset.
			uint32 _SampleOffset;

		};

		//The queued MIDI messages.
		DynamicArray<QueuedMIDIMessage> _QueuedMIDIMessages;

		//The MIDI note states.
		StaticArray<MIDINoteState, 128> _MIDINoteStates;

		/*
		*	Adds a MIDI message internally.
		*/
		FORCE_INLINE void AddMIDIMessageInternal(const MIDIMessage &message, const uint32 sample_offset) NOEXCEPT;

	};

}

//Inline.
#include "PluginGeneral.inl"
#include "PluginParameters.inl"
#include "PluginMIDI.inl"