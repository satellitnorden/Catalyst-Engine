#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicLibrary.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

/*
*	Audio effect that loads and manages a single .clap plugin.
*/
class CLAPPlugin final : public AudioEffect
{

public:

	/*
	*	Default constructor.
	*/
	CLAPPlugin() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~CLAPPlugin() NOEXCEPT;

	/*
	*	Initializes this CLAP plugin. Returns if it succeeded.
	*/
	NO_DISCARD bool Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT;

	/*
	*	Terminates this CLAP plugin.
	*/
	void Terminate() NOEXCEPT;

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
		};

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

	//The dynamic library.
	DynamicLibrary _DynamicLibrary;

	//The entry.
	const clap_plugin_entry *RESTRICT _Entry{ nullptr };

	//The host.
	clap_host_t _Host;

	//The plugin.
	const clap_plugin_t *RESTRICT _Plugin{ nullptr };

	//The parameters.
	DynamicArray<Parameter> _Parameters;

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The input events.
	DynamicArray<Event> _InputEvents;

	//The process struct.
	clap_process_t _Process;


};