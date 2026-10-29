#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Adapters/CLAP/Extensions/CLAPAudioPortsExtension.h>
#include <Audio/Plugins/Adapters/CLAP/Extensions/CLAPGUIExtension.h>
#include <Audio/Plugins/Adapters/CLAP/Extensions/CLAPNotePortsExtension.h>
#include <Audio/Plugins/Adapters/CLAP/Extensions/CLAPParamsExtension.h>
#include <Audio/Plugins/Adapters/CLAP/CLAPPluginData.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

namespace CLAPPlugin
{

	/*
	*	The 'init' function.
	*/
	bool CLAP_ABI init(const struct clap_plugin *plugin)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Initialize the editor.
		plugin_data->_Editor.Initialize(plugin_data->_Plugin);

		//Return success!
		return true;
	}

	/*
	*	The 'destroy' function.
	*/
	void CLAP_ABI destroy(const struct clap_plugin* plugin)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Destroy the editor.
		plugin_data->_Editor.DestroyEditor();

		//Destroy the plugin.
		AudioPluginGlue::Destroy(plugin_data->_Plugin);

		//Destroy the plugin data.
		delete plugin_data;
	}

	/*
	*	The 'activate' function.
	*/
	bool CLAP_ABI activate
	(
		const struct clap_plugin *plugin,
		double                    sample_rate,
		uint32_t                  min_frames_count,
		uint32_t                  max_frames_count
	)
	{
		//Cache the plugin data.
		CLAPPluginData *const RESTRICT plugin_data{ static_cast<CLAPPluginData *const RESTRICT>(plugin->plugin_data) };

		//Set the sample rate.
		plugin_data->_Plugin->SetSampleRate(static_cast<float32>(sample_rate));

		//Return success.
		return true;
	}

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

		//Set the beats per minute.
		if (process->transport)
		{
			plugin_data->_Plugin->SetBeatsPerMinute(static_cast<float32>(process->transport->tempo));
		}

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

		//Handle events.
		if (process->in_events)
		{
			const uint32_t number_of_events{ process->in_events->size(process->in_events) };

			for (uint32_t event_index{ 0 }; event_index < number_of_events; ++event_index)
			{
				const clap_event_header_t *const event_header{ process->in_events->get(process->in_events, event_index) };

				if (event_header)
				{
					switch (event_header->type)
					{
						case CLAP_EVENT_NOTE_ON:
						{
							const clap_event_note *const event{ reinterpret_cast<const clap_event_note *RESTRICT>(event_header) };

							MIDIMessage message;

							message._Type = MIDIMessage::Type::NOTE_ON;
							message._NoteOnChannel = static_cast<uint8>(event->channel);
							message._NoteOnNote = static_cast<uint8>(event->key);
							message._NoteOnVelocity = static_cast<uint8>(event->velocity * 127.0f);

							plugin_data->_Plugin->AddMIDIMessage(message, &event->header.time);

							break;
						}

						case CLAP_EVENT_NOTE_OFF:
						{
							const clap_event_note *const event{ reinterpret_cast<const clap_event_note *RESTRICT>(event_header) };

							MIDIMessage message;

							message._Type = MIDIMessage::Type::NOTE_OFF;
							message._NoteOffChannel = static_cast<uint8>(event->channel);
							message._NoteOffNote = static_cast<uint8>(event->key);

							plugin_data->_Plugin->AddMIDIMessage(message, &event->header.time);

							break;
						}

						case CLAP_EVENT_PARAM_VALUE:
						{
							const clap_event_param_value *const event{ reinterpret_cast<const clap_event_param_value *RESTRICT>(event_header) };
							plugin_data->_Plugin->SetParameterPlain(event->param_id, static_cast<float32>(event->value));

							break;
						}
					}
				}
			}
		}

		//Call the plugin's 'Process()' function!
		AudioProcessContext context;

		context._WasTimelineRunning = false; //TODO
		context._IsTimelineRunning = false; //TODO

		plugin_data->_Plugin->CallProcess(context, plugin_data->_Inputs, &plugin_data->_Outputs, number_of_channels, number_of_samples);

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

		else if (strcmp(id, CLAP_EXT_NOTE_PORTS) == 0 && CLAPNotePortsExtension::IsSupported(plugin))
		{
			return &CLAPNotePortsExtension::EXTENSION;
		}

		else if (strcmp(id, CLAP_EXT_PARAMS) == 0 && CLAPParamsExtension::IsSupported(plugin))
		{
			return &CLAPParamsExtension::EXTENSION;
		}

		//Not a supported extension.
		return nullptr;
	}

}
#endif