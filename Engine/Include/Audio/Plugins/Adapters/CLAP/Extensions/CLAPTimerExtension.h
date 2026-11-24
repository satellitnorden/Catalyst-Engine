#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/CLAP/CLAPPluginData.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

namespace CLAPTimerExtension
{

	/*
	*	The 'on_timer' function.
	*/
	void CLAP_ABI on_timer(const clap_plugin_t *plugin, clap_id timer_id)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Update the editor.
		plugin_data->_Editor.Update();
	}

	//The extension.
	constexpr clap_plugin_timer_support EXTENSION
	{
		on_timer
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