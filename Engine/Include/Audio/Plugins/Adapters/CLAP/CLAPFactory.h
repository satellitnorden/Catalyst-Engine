#if defined(AUDIO_PLUGIN_CLAP)
#pragma once
//Audio plugin.
#include <AudioPluginGlue.Generated.h>

//Audio.
#include <Audio/Plugins/Adapters/CLAP/CLAPPlugin.h>
#include <Audio/Plugins/Adapters/CLAP/CLAPPluginData.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

/*
*	The 'get_plugin_count' function.
*/
uint32_t CLAP_ABI get_plugin_count(const struct clap_plugin_factory* factory)
{
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
	CLAP_VERSION,						//clap_version
	AudioPluginGlue::GetIdentifier(),	//id
	AudioPluginGlue::GetName(),			//name
	AudioPluginGlue::GetVendor(),		//vendor
	nullptr,							//url
	nullptr,							//manual_url
	nullptr,							//support_url
	"1.0",								//version
	"A plugin. (:",						//description
	PLUGIN_FEATURES						//features
};

const clap_plugin_descriptor_t *CLAP_ABI get_plugin_descriptor(const struct clap_plugin_factory *factory, uint32_t index)
{
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
	clap_plugin_t *const plugin = new clap_plugin_t();

	plugin->desc = &PLUGIN_DESCRIPTOR;
	plugin->plugin_data = new CLAPPluginData(AudioPluginGlue::Create());
	plugin->init = CLAPPlugin::init;
	plugin->destroy = CLAPPlugin::destroy;
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
	plugin->process = CLAPPlugin::process;
	plugin->get_extension = CLAPPlugin::get_extension;
	plugin->on_main_thread = [](const struct clap_plugin* plugin)
	{

	};

	return plugin;
}

//The plugin factory.
const clap_plugin_factory PLUGIN_FACTORY
{
	get_plugin_count,
	get_plugin_descriptor,
	create_plugin
};
#endif