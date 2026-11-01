#if defined(AUDIO_PLUGIN_VST3)
//Audio.
#include <Audio/Plugins/Adapters/VST3/VST3DebugLogging.h>
#include <Audio/Plugins/Adapters/VST3/VST3PluginFactory.h>

//Platform.
#if defined(CATALYST_PLATFORM_WINDOWS)
#include <Platform/Windows/CatalystPlatformWindows.h>
BOOL WINAPI DllMain
(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved
)
{
	VST3_DEBUG_LOG("DllMain called!");

    CatalystPlatformWindows::_Instance = hinstDLL;

	return TRUE;
}
#endif

/*
*	The entry point.
*/
extern "C"
{
	//The factory.
	VST3PluginFactory FACTORY;

	SMTG_EXPORT_SYMBOL Steinberg::IPluginFactory *PLUGIN_API GetPluginFactory()
	{
		VST3_DEBUG_LOG("GetPluginFactory called!");

		//Return the factory.
		return &FACTORY;
	}
}
#endif