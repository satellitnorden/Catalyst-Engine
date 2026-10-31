#if defined(AUDIO_PLUGIN_VST3)
//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3PluginFactory.h>

//The factory.
VST3PluginFactory FACTORY;

/*
*	The entry point.
*/
SMTG_EXPORT_SYMBOL Steinberg::IPluginFactory *PLUGIN_API GetPluginFactory()
{
	//Return the factory.
	return &FACTORY;
}
#endif