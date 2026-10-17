#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/CLAP/CLAPPluginData.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

//STD.
#include <cstdio>

namespace CLAPGUIExtension
{

	/*
	*	The 'is_api_supported' function.
	*/
	bool CLAP_ABI is_api_supported(const clap_plugin_t *plugin, const char *api, bool is_floating)
	{
		return strcmp(api, CLAP_WINDOW_API_WIN32) == 0 && !is_floating;
	}

	/*
	*	The 'get_preferred_api' function.
	*/
	bool CLAP_ABI get_preferred_api
	(
		const clap_plugin_t *plugin,
		const char **api,
		bool *is_floating
	)
	{
		*api = CLAP_WINDOW_API_WIN32;
		*is_floating = false;

		return true;
	}

	/*
	*	The 'create' function.
	*/
	bool CLAP_ABI create(const clap_plugin_t *plugin, const char *api, bool is_floating)
	{
		//Don't think we need to do anything here yet?
		return true;
	}

	/*
	*	The 'destroy' function.
	*/
	void CLAP_ABI destroy(const clap_plugin_t *plugin)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Destroy the window.
		plugin_data->_PluginWindow.Destroy();
	}

	/*
	*	The 'set_scale' function.
	*/
	bool CLAP_ABI set_scale(const clap_plugin_t *plugin, double scale)
	{
		//We lie a bit and say that it's okay here, but for now, completely ignore scale.
		return true;
	}

	/*
	*	The 'get_size' function.
	*/
	bool CLAP_ABI get_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Retrieve the resolution.
		const Resolution resolution{ plugin_data->_Plugin->GetResolution() };

		//Set the width and height and return success.
		*width = resolution._Width;
		*height = resolution._Height;

		return true;
	}

	/*
	*	The 'can_resize' function.
	*/
	bool CLAP_ABI can_resize(const clap_plugin_t *plugin)
	{
		return false;
	}

	/*
	*	The 'get_resize_hints' function.
	*/
	bool CLAP_ABI get_resize_hints(const clap_plugin_t *plugin, clap_gui_resize_hints_t *hints)
	{
		return false;
	}

	/*
	*	The 'adjust_size' function.
	*/
	bool CLAP_ABI adjust_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height)
	{
		return false;
	}

	/*
	*	The 'set_size' function.
	*/
	bool CLAP_ABI set_size(const clap_plugin_t *plugin, uint32_t width, uint32_t height)
	{
		return false;
	}

	/*
	*	The 'set_parent' function.
	*/
	bool CLAP_ABI set_parent(const clap_plugin_t *plugin, const clap_window_t *window)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Return if the child window was created successfully.
		return plugin_data->_PluginWindow.Create(window->win32, plugin_data->_Plugin->GetResolution());
	}

	/*
	*	The 'set_transient' function.
	*/
	bool CLAP_ABI set_transient(const clap_plugin_t *plugin, const clap_window_t *window)
	{
		return false;
	}

	/*
	*	The 'suggest_title' function.
	*/
	void CLAP_ABI suggest_title(const clap_plugin_t *plugin, const char *title)
	{
		int x = 0;
	}

	/*
	*	The 'show' function.
	*/
	bool CLAP_ABI show(const clap_plugin_t *plugin)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Show the window.
		plugin_data->_PluginWindow.Show();

		//Return success!
		return true;
	}

	/*
	*	The 'hide' function.
	*/
	bool CLAP_ABI hide(const clap_plugin_t *plugin)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Hide the window.
		plugin_data->_PluginWindow.Hide();

		//Return success!
		return true;
	}

	//The extension.
	constexpr clap_plugin_gui_t EXTENSION
	{
		is_api_supported,
		get_preferred_api,
		create,
		destroy,
		set_scale,
		get_size,
		can_resize,
		get_resize_hints,
		adjust_size,
		set_size,
		set_parent,
		set_transient,
		suggest_title,
		show,
		hide
	};

	/*
	*	Returns whether or not the given plugin supports this extension.
	*/
	FORCE_INLINE NO_DISCARD bool IsSupported(const clap_plugin_t *plugin) NOEXCEPT
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//This extension is supported if the resolution is non-zero.
		const Resolution resolution{ plugin_data->_Plugin->GetResolution() };

		return resolution._Width > 0 && resolution._Height > 0;
	}

}
#endif