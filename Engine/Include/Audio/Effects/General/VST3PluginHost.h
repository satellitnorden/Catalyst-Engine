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
	constexpr static uint64 IMPLEMENTATION_SIZE{ 96 };

	/*
	*	Default constructor.
	*/
	VST3PluginHost() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~VST3PluginHost() NOEXCEPT;

	/*
	*	Initializes this VST3 plugin.
	*/
	void Initialize(const char *const RESTRICT plugin_file_path) NOEXCEPT;

	/*
	*	Terminates this VST3 plugin.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Sets a parameter with the given identifier to the given value.
	*/
	void SetParameter(const char *const RESTRICT name, const float64 value) NOEXCEPT;

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

};