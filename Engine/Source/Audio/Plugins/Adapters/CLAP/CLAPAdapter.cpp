#if defined(AUDIO_PLUGIN_CLAP)
//Audio.
#include <Audio/Plugins/Adapters/CLAP/CLAPFactory.h>

//Platform.
#if defined(CATALYST_PLATFORM_WINDOWS)
#include <Platform/Windows/CatalystPlatformWindows.h>
#endif

//Third party.
#include <ThirdParty/CLAP/clap.h>

//STD.
#include <cstring>
#include <filesystem>
#include <fstream>

/*
*	The 'init' function.
*/
bool CLAP_ABI init(const char *plugin_path)
{
	//Retrieve platform-specific stuff.
#if defined(CATALYST_PLATFORM_WINDOWS)
	HMODULE module = nullptr;

	if (!GetModuleHandleExW
	(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCWSTR>(&init),
		&module
	))
	{
		return false;
	}

	CatalystPlatformWindows::_Instance = module;
#endif

	return true;
}

/*
*	The 'deinit' function.
*/
void CLAP_ABI deinit()
{
	
}

/*
*	The 'get_factory' function.
*/
const void *CLAP_ABI get_factory(const char *factory_id)
{
	if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) != 0)
	{
		return nullptr;
	}

	return &PLUGIN_FACTORY;
}

//Export the CLAP plugin entry object.
extern "C" CLAP_EXPORT const clap_plugin_entry_t clap_entry = 
{
	CLAP_VERSION,	//clap_version
	init,			//init
	deinit,			//deinit
	get_factory		//get_factory
};
#endif