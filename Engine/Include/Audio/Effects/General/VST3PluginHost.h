#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

//Audio.
#include <Audio/Effects/General/PluginHost.h>

/*
*	Audio effect that loads and manages a single .vst3 plugin.
*/
class VST3PluginHost final : public PluginHost
{

public:

	//Constants.
	constexpr static uint64 IMPLEMENTATION_SIZE{ 192 };

	/*
	*	Default constructor.
	*/
	VST3PluginHost() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VST3PluginHost() NOEXCEPT;

	/*
	*	Initializes this plugin host. Returns if it succeeded.
	*/
	NO_DISCARD bool Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT override;

	/*
	*	Sets a parameter (normalized) with the given identifier. Returns if it succeeded.
	*/
	NO_DISCARD bool SetParameterNormalized(const HashString identifier, const float64 value) NOEXCEPT override;

	/*
	*	Sets a parameter (plain) with the given identifier. Returns if it succeeded.
	*/
	NO_DISCARD bool SetParameterPlain(const HashString identifier, const float64 value) NOEXCEPT override;

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

private:

	//The implementation.
	Any<IMPLEMENTATION_SIZE> _Implementation;

	/*
	*	Returns the implementation.
	*/
	NO_DISCARD class VST3PluginHostImplementation *const RESTRICT Implementation() NOEXCEPT;

	/*
	*	Creates the edit controller.
	*/
	NO_DISCARD bool CreateEditController() NOEXCEPT;

	/*
	*	Initializes the edit controller.
	*/
	NO_DISCARD bool InitializeEditController() NOEXCEPT;

	/*
	*	Terminates the edit controller.
	*/
	NO_DISCARD bool TerminateEditController() NOEXCEPT;

	/*
	*	Destroys the edit controller.
	*/
	NO_DISCARD bool DestroyEditController() NOEXCEPT;

};