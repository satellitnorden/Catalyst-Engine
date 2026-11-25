#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

//Audio.
#include <Audio/Effects/General/PluginHost.h>

/*
*	Audio effect that loads and manages a single .clap plugin.
*/
class CLAPPluginHost final : public PluginHost
{

public:

	//Constants.
	constexpr static uint64 IMPLEMENTATION_SIZE{ 2'048 };

	/*
	*	Default constructor.
	*/
	CLAPPluginHost() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~CLAPPluginHost() NOEXCEPT;

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
	*	Initializes this plugin host. Returns if it succeeded.
	*/
	NO_DISCARD bool Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT override;

	/*
	*	Returns the parameter (plain) with the given identifier. Returns if it succeeded.
	*/
	NO_DISCARD bool GetParameterPlain(const HashString identifier, float64 *const RESTRICT value) NOEXCEPT override;

	/*
	*	Sets a parameter (plain) with the given identifier. Returns if it succeeded.
	*/
	NO_DISCARD bool SetParameterPlain(const HashString identifier, const float64 value) NOEXCEPT override;

	/*
	*	Returns the parameter (normalized) with the given identifier. Returns if it succeeded.
	*/
	NO_DISCARD bool GetParameterNormalized(const HashString identifier, float64 *const RESTRICT value) NOEXCEPT override;

	/*
	*	Sets a parameter (normalized) with the given identifier. Returns if it succeeded.
	*/
	NO_DISCARD bool SetParameterNormalized(const HashString identifier, const float64 value) NOEXCEPT override;

	/*
	*	Shows the UI. Returns if it succeeded.
	*/
	NO_DISCARD bool ShowUI() NOEXCEPT override;

	/*
	*	Hides the UI. Returns if it succeeded.
	*/
	NO_DISCARD bool HideUI() NOEXCEPT override;

	/*
	*	Returns the state. Returns if it succeeded.
	*/
	NO_DISCARD bool GetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT override;

	/*
	*	Sets the state. Returns if it succeeded.
	*/
	NO_DISCARD bool SetState(StreamArchive *const RESTRICT stream_archive) NOEXCEPT override;

	/*
	*	Terminates this VST3 plugin.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Callback for when this plugin receives MIDI messages.
	*/
	void OnMIDIMessages(const MIDIMessage *const RESTRICT messages, const uint64 number_of_messages) NOEXCEPT override;

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	void Process
	(
		const AudioProcessContext& context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override;

private:

	//The implementation.
	Any<IMPLEMENTATION_SIZE> _Implementation;

	/*
	*	Returns the implementation.
	*/
	NO_DISCARD class CLAPPluginHostImplementation *const RESTRICT Implementation() NOEXCEPT;

	/*
	*	Returns the extension with the given identifier (if supported)
	*/
	NO_DISCARD const void *const RESTRICT GetExtension(const char *const RESTRICT identifier) NOEXCEPT;

	/*
	*	Returns the audio ports extension.
	*/
	NO_DISCARD const void *const RESTRICT GetAudioPortsExtension() NOEXCEPT;

	/*
	*	Returns the GUI extension.
	*/
	NO_DISCARD const void *const RESTRICT GetGUIExtension() NOEXCEPT;

	/*
	*	Returns the log extension.
	*/
	NO_DISCARD const void *const RESTRICT GetLogExtension() NOEXCEPT;

	/*
	*	Returns the thread check extension.
	*/
	NO_DISCARD const void *const RESTRICT GetThreadCheckExtension() NOEXCEPT;

	/*
	*	Processes the incoming parameter value changes.
	*/
	void ProcessIncomingParameterValueChanges() NOEXCEPT;

};