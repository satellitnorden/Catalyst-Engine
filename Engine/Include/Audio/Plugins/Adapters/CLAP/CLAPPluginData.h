#if defined(AUDIO_PLUGIN_CLAP)
#pragma once

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Editor/PluginEditor.h>

//Third party.
#include <ThirdParty/CLAP/clap.h>

/*
*	CLAP Plugin Data class definition.
*/
class CLAPPluginData final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE CLAPPluginData(Audio::Plugin *const RESTRICT plugin, const clap_host_t *const RESTRICT host) NOEXCEPT
		:
		_Plugin(plugin),
		_Host(host)
	{
		
	}

	//The plugin.
	Audio::Plugin *RESTRICT _Plugin;

	//The host.
	const clap_host_t *RESTRICT _Host;

	//The host timer support extension.
	const clap_host_timer_support *RESTRICT _HostTimerSupportExtension{ nullptr };

	//The timer identifier.
	clap_id _TimerIdentifier{ CLAP_INVALID_ID };

	//Denotes whether or not a timer is registered.
	bool _TimerRegistered{ false };

	//The editor.
	Audio::PluginEditor _Editor;

	//The inputs.
	DynamicArray<DynamicArray<float32>> _Inputs;

	//The outputs.
	DynamicArray<DynamicArray<float32>> _Outputs;

	/*
	*	Registers the timer.
	*/
	FORCE_INLINE void RegisterTimer() NOEXCEPT
	{
		if (_HostTimerSupportExtension && !_TimerRegistered)
		{
			const bool could_register{ _HostTimerSupportExtension->register_timer(_Host, 16, &_TimerIdentifier) };

			if (could_register)
			{
				_TimerRegistered = true;
			}
		}
	}

	/*
	*	Unregisters the timer.
	*/
	FORCE_INLINE void UnregisterTimer() NOEXCEPT
	{
		if (_HostTimerSupportExtension && _TimerRegistered)
		{
			const bool could_unregister{ _HostTimerSupportExtension->unregister_timer(_Host, _TimerIdentifier) };

			if (could_unregister)
			{
				_TimerRegistered = false;
			}
		}
	}

};
#endif