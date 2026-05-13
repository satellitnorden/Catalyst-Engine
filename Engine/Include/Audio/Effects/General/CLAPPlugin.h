#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicLibrary.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

//Concurrency.
#include <Concurrency/AtomicFlag.h>
#include <Concurrency/AtomicQueue.h>
#include <Concurrency/Task.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

/*
*	Audio effect that loads and manages a single .clap plugin.
*/
class CLAPPlugin final : public AudioEffect
{

public:

	/*
	*	Main thread request class definition.
	*/
	class MainThreadRequest final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			NONE,

			FINALIZE_PLUGIN,
			UPDATE_PLUGIN,
			SHOW_GUI,
			HIDE_GUI
		};

		//The type.
		Type _Type{ Type::NONE };
	};

	/*
	*	Audio thread request class definition.
	*/
	class AudioThreadRequest final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			NONE,

			START_PROCESSING,
			SET_PARAMETER
		};

		//The type.
		Type _Type{ Type::NONE };

		//The data.
		union
		{
			struct
			{
				//The identifier.
				HashString _Identifier;

				//The value.
				float64 _Value;
			} _SetParameterData;
		};

		/*
		*	Default constructor.
		*/
		FORCE_INLINE AudioThreadRequest() NOEXCEPT
		{
			Memory::Set(this, 0, sizeof(AudioThreadRequest));
		}

	};

	/*
	*	Default constructor.
	*/
	CLAPPlugin() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~CLAPPlugin() NOEXCEPT;

	/*
	*	Initializes this CLAP plugin.
	*/
	void Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT;

	/*
	*	Terminates this CLAP plugin.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns if this audio effect wants updates on the main thread.
	*/
	FORCE_INLINE NO_DISCARD bool WantsMainThreadUpdate() NOEXCEPT override
	{
		return true;
	}

	/*
	*	Updates this audio effect on the main thread.
	*/
	void MainThreadUpdate() NOEXCEPT override;

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override;

	/*
	*	Adds a main thread request.
	*/
	void AddMainThreadRequest(const MainThreadRequest request) NOEXCEPT;

	/*
	*	Adds an audio thread request.
	*/
	void AddAudioThreadRequest(const AudioThreadRequest request) NOEXCEPT;

	/*
	*	Sets a parameter with the given identifier to the given value.
	*/
	void SetParameter(const HashString identifier, const float64 value) NOEXCEPT;

private:

	/*
	*	Event class definition.
	*/
	class Event final
	{

	public:

		//Store all possible event types as a union.
		union
		{
			clap_event_param_value_t _ParameterValue;
			clap_event_param_gesture_t _GestureValue;
		};

		//The type.
		uint16_t type;

	};

	/*
	*	Parameter class definition.
	*/
	class Parameter final
	{

	public:

		//The name.
		DynamicString _Name;

		//The identifier.
		HashString _Identifier;

		//The ID.
		clap_id _ID;

		//The cookie.
		void *RESTRICT _Cookie;

		//The minimum value.
		float64 _MinimumValue;

		//The maximum value.
		float64 _MaximumValue;

		//The default value.
		float64 _DefaultValue;

	};

	/*
	*	GUI class definition.
	*/
	class GUI final
	{

	public:

		//The extension.
		const clap_plugin_gui_t *RESTRICT _Extension{ nullptr };

		//The window.
		void *RESTRICT _Window{ nullptr };

	};

	//The plugin file path.
	DynamicString _PluginFilePath;

	//Denotes whether or not this plugin is ready.
	AtomicFlag _PluginReady;

	//The initialization task.
	Task _InitializationTask;

	//The dynamic library.
	DynamicLibrary _DynamicLibrary;

	//The entry.
	const clap_plugin_entry *RESTRICT _Entry{ nullptr };

	//The factory.
	const clap_plugin_factory *RESTRICT _Factory{ nullptr };

	//The host.
	clap_host_t _Host;

	//The plugin.
	const clap_plugin_t *RESTRICT _Plugin{ nullptr };

	//The parameters.
	DynamicArray<Parameter> _Parameters;

	//The main thread requests.
	AtomicQueue<MainThreadRequest, 1'024, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _MainThreadRequests;

	//The audio thread requests.
	AtomicQueue<AudioThreadRequest, 1'024, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _AudioThreadRequests;

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The input events.
	DynamicArray<Event> _InputEvents;

	//The output events.
	DynamicArray<Event> _OutputEvents;

	//The process struct.
	clap_process_t _Process;

	//The GUI.
	GUI _GUI;

	/*
	*	Initializes internally.
	*/
	void InitializeInternal(const char *const RESTRICT plugin_file_path) NOEXCEPT;

	/*
	*	Finalizes the plugin.
	*/
	void FinalizePlugin() NOEXCEPT;


};