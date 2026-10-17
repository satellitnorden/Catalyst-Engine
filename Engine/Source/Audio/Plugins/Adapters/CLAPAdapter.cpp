#if defined(AUDIO_PLUGIN_CLAP)
//Audio plugin.
#include <AudioPluginGlue.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

//STD.
#include <cstring>
#include <filesystem>
#include <fstream>

void DebugLog(const char *message)
{
	constexpr const char *FILE_PATH{ "C:\\Users\\Denni\\My Drive\\Share Folder\\Plugins\\TEST\\Log.txt" };

	if (!std::filesystem::exists(FILE_PATH))
	{
		std::ofstream file{ FILE_PATH };

		file << "Start!" << std::endl;

		file.close();
	}

	std::ofstream file{ FILE_PATH, std::ofstream::app };

	file << message << std::endl;

	file.close();
}

/*
*	The 'get_plugin_count' function.
*/
uint32_t CLAP_ABI get_plugin_count(const struct clap_plugin_factory* factory)
{
	DebugLog("get_plugin_count");

	return 1;
}

/*
*	The 'get_plugin_descriptor' function.
*/
const char *PLUGIN_FEATURES[]
{
	CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
	nullptr
};
const clap_plugin_descriptor_t PLUGIN_DESCRIPTOR
{
	CLAP_VERSION,					//clap_version
	AudioPluginGlue::Identifier(),	//id
	AudioPluginGlue::Name(),		//name
	AudioPluginGlue::Vendor(),		//vendor
	nullptr,						//url
	nullptr,						//manual_url
	nullptr,						//support_url
	"1.0",							//version
	"A plugin. (:",					//description
	PLUGIN_FEATURES					//features
};

const clap_plugin_descriptor_t *CLAP_ABI get_plugin_descriptor(const struct clap_plugin_factory *factory, uint32_t index)
{
	DebugLog("get_plugin_descriptor");

	if (index != 0)
	{
		return nullptr;
	}

	return &PLUGIN_DESCRIPTOR;
}

/*
*	The 'create_plugin' function.
*/
const clap_plugin_t *CLAP_ABI create_plugin(const struct clap_plugin_factory *factory, const clap_host_t *host, const char *plugin_id)
{
	DebugLog("create_plugin");

	clap_plugin_t *const plugin = new clap_plugin_t();

	plugin->desc = &PLUGIN_DESCRIPTOR;
	plugin->plugin_data = AudioPluginGlue::Create();
	plugin->init = [](const struct clap_plugin *plugin)
	{
		return true;
	};
	plugin->destroy = [](const struct clap_plugin *plugin)
	{
		
	};
	plugin->activate = []
	(
		const struct clap_plugin *plugin,
		double                    sample_rate,
		uint32_t                  min_frames_count,
		uint32_t                  max_frames_count
	)
	{
		return true;
	};
	plugin->deactivate = [](const struct clap_plugin *plugin)
	{
		
	};
	plugin->start_processing = [](const struct clap_plugin *plugin)
	{
		return true;
	};
	plugin->stop_processing = [](const struct clap_plugin *plugin)
	{
		
	};
	plugin->reset = [](const struct clap_plugin *plugin)
	{
	
	};
	plugin->process = [](const struct clap_plugin *plugin, const clap_process_t *process) -> clap_process_status
	{
		return CLAP_PROCESS_CONTINUE;
	};
	plugin->get_extension = [](const struct clap_plugin *plugin, const char *id) -> const void*
	{
		return nullptr;
	};
	plugin->on_main_thread = [](const struct clap_plugin *plugin)
	{
		
	};

	return plugin;
}

/*
*	The 'init' function.
*/
bool CLAP_ABI init(const char *plugin_path)
{
	DebugLog("init");

	return true;
}

/*
*	The 'deinit' function.
*/
void CLAP_ABI deinit()
{
	DebugLog("deinit");
}

/*
*	The 'get_factory' function.
*/
const clap_plugin_factory PLUGIN_FACTORY
{
	get_plugin_count,
	get_plugin_descriptor,
	create_plugin
};

const void *CLAP_ABI get_factory(const char *factory_id)
{
	DebugLog("get_factory");

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