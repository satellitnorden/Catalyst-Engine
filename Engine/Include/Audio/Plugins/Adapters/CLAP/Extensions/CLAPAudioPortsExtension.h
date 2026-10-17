#if defined(AUDIO_PLUGIN_CLAP)
#pragma once
//Third party.
#include <ThirdParty/CLAP/clap.h>

//STD.
#include <cstdio>

namespace CLAPAudioPortsExtension
{

	/*
	*	The 'count' function.
	*/
	uint32_t count(const clap_plugin_t *plugin, bool is_input)
	{
		return 1;
	}

	/*
	*	The 'get' function.
	*/
	bool get
	(
		const clap_plugin_t *plugin,
		uint32_t                index,
		bool                    is_input,
		clap_audio_port_info_t *info
	)
	{
		if (index >= 1)
		{
			return false;
		}

		info->id = is_input ? 0 : 1;

		if (is_input)
		{
			sprintf_s(info->name, "Stereo Input");
		}

		else
		{
			sprintf_s(info->name, "Stereo Output");
		}

		info->flags = CLAP_AUDIO_PORT_IS_MAIN;
		info->channel_count = 2;
		info->port_type = CLAP_PORT_STEREO;
		info->in_place_pair = is_input ? 1 : 0;

		return true;
	}

	//The extension.
	constexpr clap_plugin_audio_ports_t EXTENSION
	{
		count,
		get
	};

}
#endif