#if defined(AUDIO_PLUGIN_CLAP)
//Audio.
#include <Audio/Plugins/Adapters/CLAP/CLAPFactory.h>

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