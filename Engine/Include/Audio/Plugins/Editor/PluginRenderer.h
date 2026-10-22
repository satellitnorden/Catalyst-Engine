#if defined(AUDIO_PLUGIN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Audio.
#include <Audio/Plugins/Editor/PluginWindow.h>

//UI.
#include <UI/Core/RenderCommand.h>

namespace Audio
{

	/*
	*	Class responsible for rendering a list of render commands for a plugin.
	*/
	class PluginRenderer
	{

	public:

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~PluginRenderer() NOEXCEPT
		{

		}

		/*
		*	Initializes this plugin renderer.
		*/
		FORCE_INLINE virtual void Initialize(const Resolution resolution, Audio::PluginWindow *const RESTRICT window) NOEXCEPT
		{

		}

		/*
		*	Renders.
		*/
		FORCE_INLINE virtual void Render(const DynamicArray<UI::RenderCommand> &render_commands) NOEXCEPT = 0;

		/*
		*	Terminates this plugin renderer.
		*/
		FORCE_INLINE virtual void Terminate() NOEXCEPT
		{

		}

	};

}
#endif