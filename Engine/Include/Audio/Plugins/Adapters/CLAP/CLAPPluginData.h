#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Audio.
#include <Audio/Plugins/Core/Plugin.h>

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

	//The parent window.
	void *RESTRICT _ParentWindow{ nullptr };

	//The child window.
	void *RESTRICT _ChildWindow{ nullptr };

};
#endif