#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

//STD.
#include <cstdio>

namespace CLAPNotePortsExtension
{

	/*
	*	The 'count' function.
	*/
	uint32_t CLAP_ABI count(const clap_plugin_t *plugin, bool is_input)
	{
		return is_input ? 1 : 0;
	}

	/*
	*	The 'get' function.
	*/
	bool CLAP_ABI get
	(
		const clap_plugin_t *plugin,
		uint32_t               index,
		bool                   is_input,
		clap_note_port_info_t *info
	)
	{
		//Do some error checking.
		if (index > 0)
		{
			return false;
		}

		if (!is_input)
		{
			return false;
		}

		//Fill in the info.
		info->id = 0;
		info->supported_dialects = CLAP_NOTE_DIALECT_CLAP;
		info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
		sprintf_s(info->name, "Default MIDI Port");
		
		//Return success!
		return true;
	}

	//The extension.
	constexpr clap_plugin_note_ports EXTENSION
	{
		count,
		get
	};

	/*
	*	Returns whether or not the given plugin supports this extension.
	*/
	FORCE_INLINE NO_DISCARD bool IsSupported(const clap_plugin_t *plugin) NOEXCEPT
	{
		//This extension is always supported.
		return true;
	}

}
#endif