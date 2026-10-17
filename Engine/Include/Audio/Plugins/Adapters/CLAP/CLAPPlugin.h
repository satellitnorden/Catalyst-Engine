#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Adapters/CLAP/Extensions/CLAPAudioPortsExtension.h>
#include <Audio/Plugins/Adapters/CLAP/Extensions/CLAPGUIExtension.h>
#include <Audio/Plugins/Adapters/CLAP/CLAPPluginData.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

/*
*	The 'process' function.
*/
clap_process_status CLAP_ABI process(const struct clap_plugin *plugin, const clap_process_t *process)
{
	//Do some sanity checking.
	if (process->audio_inputs_count == 0 || process->audio_outputs_count == 0)
	{
		return CLAP_PROCESS_ERROR;
	}

	//Cache some data.
	CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };
	const uint8 number_of_channels{ static_cast<uint8>(process->audio_inputs[0].channel_count) };
	const uint32 number_of_samples{ process->frames_count };

	//Set up the inputs/outputs.
	plugin_data->_Inputs.Resize<true>(number_of_channels);
	plugin_data->_Outputs.Resize<true>(number_of_channels);

	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		plugin_data->_Inputs.At(channel_index).Resize<false>(number_of_samples);
		plugin_data->_Outputs.At(channel_index).Resize<false>(number_of_samples);

		Memory::Copy(plugin_data->_Inputs.At(channel_index).Data(), process->audio_inputs[0].data32[channel_index], number_of_samples * sizeof(float32));
		Memory::Set(plugin_data->_Outputs.At(channel_index).Data(), 0, number_of_samples * sizeof(float32));
	}

	//Call the plugin's 'Process()' function!
	AudioProcessContext context;

	context._WasTimelineRunning = false; //TODO
	context._IsTimelineRunning = false; //TODO

	plugin_data->_Plugin->Process(context, plugin_data->_Inputs, &plugin_data->_Outputs, number_of_channels, number_of_samples);

	//Copy the outputs.
	for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
	{
		Memory::Copy(process->audio_outputs[0].data32[channel_index], plugin_data->_Outputs.At(channel_index).Data(), number_of_samples * sizeof(float32));
	}

	//We can continue from here!
	return CLAP_PROCESS_CONTINUE;
}

/*
*	The 'get_extension' function.
*/
const void *get_extension(const struct clap_plugin *plugin, const char *id)
{
	//Cache the plugin data.
	CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

	//Query for supported extensions.
	if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0 && CLAPAudioPortsExtension::IsSupported(plugin))
	{
		return &CLAPAudioPortsExtension::EXTENSION;
	}

	else if (strcmp(id, CLAP_EXT_GUI) == 0 && CLAPGUIExtension::IsSupported(plugin))
	{
		return &CLAPGUIExtension::EXTENSION;
	}

	//Not a supported extension.
	return nullptr;
}
#endif