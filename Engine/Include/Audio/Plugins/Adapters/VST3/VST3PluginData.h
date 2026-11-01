#if defined(AUDIO_PLUGIN_VST3)
#pragma once

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Editor/PluginEditor.h>

//Audio plugin.
#include <AudioPluginGlue.Generated.h>

/*
*	VST3 Plugin Data class definition.
*/
class VST3PluginData final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE VST3PluginData() NOEXCEPT
	{
		//Create the plugin.
		_Plugin = AudioPluginGlue::Create();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~VST3PluginData() NOEXCEPT
	{
		//Destroy the plugin.
		AudioPluginGlue::Destroy(_Plugin);
	}

	//The plugin.
	Audio::Plugin *RESTRICT _Plugin;

	//The editor.
	Audio::PluginEditor _Editor;

	//The inputs.
	DynamicArray<DynamicArray<float32>> _Inputs;

	//The outputs.
	DynamicArray<DynamicArray<float32>> _Outputs;

};
#endif