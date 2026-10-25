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

namespace CLAPParamsExtension
{

	/*
	*	The 'count' function.
	*/
	uint32_t CLAP_ABI count(const clap_plugin_t *plugin)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Return the number of parameters.
		return static_cast<uint32_t>(plugin_data->_Plugin->GetNumberOfParameters());
	}

	/*
	*	The 'get_info' function.
	*/
	bool CLAP_ABI get_info
	(
		const clap_plugin_t	*plugin,
		uint32_t			param_index,
		clap_param_info_t	*param_info
	)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Fill in the info.
		param_info->id = param_index;
		param_info->flags = 0;

		if (plugin_data->_Plugin->IsParameterStepped(param_index))
		{
			param_info->flags |= CLAP_PARAM_IS_STEPPED;
		}

		if (plugin_data->_Plugin->IsParameterEnumeration(param_index))
		{
			param_info->flags |= CLAP_PARAM_IS_ENUM;
		}

		param_info->cookie = nullptr; //TODO!
		sprintf_s(param_info->name, plugin_data->_Plugin->GetParameterName(param_index));
		sprintf_s(param_info->module, plugin_data->_Plugin->GetParameterName(param_index));
		param_info->min_value = static_cast<double>(plugin_data->_Plugin->GetParameterMinimum(param_index));
		param_info->max_value = static_cast<double>(plugin_data->_Plugin->GetParameterMaximum(param_index));
		param_info->default_value = static_cast<double>(plugin_data->_Plugin->GetParameterDefault(param_index));

		//Return the number of parameters.
		return static_cast<uint32_t>(plugin_data->_Plugin->GetNumberOfParameters());
	}

	/*
	*	The 'get_value' function.
	*/
	bool CLAP_ABI get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Write the value.
		*out_value = static_cast<double>(plugin_data->_Plugin->GetParameterCurrentPlain(param_id));

		//Return success.
		return true;
	}

	/*
	*	The 'value_to_text' function.
	*/
	bool CLAP_ABI value_to_text
	(
		const clap_plugin_t	*plugin,
		clap_id				param_id,
		double				value,
		char				*out_buffer,
		uint32_t			out_buffer_capacity
	)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		///Print the string.
		return plugin_data->_Plugin->PrintParameterValueString(param_id, out_buffer, out_buffer_capacity, static_cast<float32>(value));
	}

	/*
	*	The 'text_to_value' function.
	*/
	bool CLAP_ABI text_to_value
	(
		const clap_plugin_t *plugin,
		clap_id             param_id,
		const char			*param_value_text,
		double				*out_value)
	{
		//TODO!
		return false;
	}

	/*
	*	The 'flush' function.
	*/
	void CLAP_ABI flush
	(
		const clap_plugin_t			*plugin,
		const clap_input_events_t	*in,
		const clap_output_events_t	*out)
	{
		//TODO!
		
	}

	//The extension.
	constexpr clap_plugin_params_t EXTENSION
	{
		count,
		get_info,
		get_value,
		value_to_text,
		text_to_value,
		flush
	};

	/*
	*	Returns whether or not the given plugin supports this extension.
	*/
	FORCE_INLINE NO_DISCARD bool IsSupported(const clap_plugin_t *plugin) NOEXCEPT
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//This extension is supported if the number of parameters is non-zero.
		return plugin_data->_Plugin->GetNumberOfParameters() > 0;
	}

}
#endif