#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Adapters/Core/PluginWindow.h>

/*
*	CLAP Plugin Data class definition.
*/
class CLAPPluginData final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CLAPPluginData(Audio::Plugin *const RESTRICT plugin) NOEXCEPT
		:
		_Plugin(plugin)
	{

	}

	//The plugin.
	Audio::Plugin *RESTRICT _Plugin;

	//The inputs.
	DynamicArray<DynamicArray<float32>> _Inputs;

	//The outputs.
	DynamicArray<DynamicArray<float32>> _Outputs;

	//The plugin window.
	PluginWindow _PluginWindow;

};
#endif