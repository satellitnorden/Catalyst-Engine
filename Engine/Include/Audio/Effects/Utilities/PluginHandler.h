#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Plugins/Core/Plugin.h>

/*
*	Plugin handler audio effect.
*	Utility effect that holds an audio plugin internally and executes it.
*/
class PluginHandler final : public AudioEffect
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PluginHandler(Audio::Plugin *const RESTRICT plugin) NOEXCEPT
		:
		_Plugin(plugin)
	{

	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override
	{
		//Call the plugin's process function.
		_Plugin->CallProcess(context, inputs, outputs, number_of_channels, number_of_samples);
	}

private:

	//The plugin.
	Audio::Plugin *RESTRICT _Plugin;

};