#if defined(AUDIO_PLUGIN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DeltaTimer.h>

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Editor/PluginRenderer.h>
#include <Audio/Plugins/Editor/PluginWindow.h>

//Systems.
#include <Systems/UISystem.h>

namespace Audio
{

	/*
	*	Class handling all of the editor functionality for a given plugin,
	*	such as window creation, UI updates and rendering.
	*/
	class PluginEditor final
	{

	public:

		//Constants.
#if defined(AUDIO_PLUGIN_STANDALONE)
		constexpr static bool USE_INTERNAL_RENDERING{ true };
#else
		constexpr static bool USE_INTERNAL_RENDERING{ true };
#endif

		/*
		*	Initializes this plugin editor.
		*/
		void Initialize(Audio::Plugin *const RESTRICT plugin) NOEXCEPT;

		/*
		*	Creates the editor.
		*/
		void CreateEditor(void *const RESTRICT parent_window) NOEXCEPT;

		/*
		*	Destroys the editor.
		*/
		void DestroyEditor() NOEXCEPT;

		/*
		*	Shows the editor window.
		*/
		FORCE_INLINE void ShowEditorWindow() NOEXCEPT
		{
			_Window.Show();
		}

		/*
		*	Hides the editor window.
		*/
		FORCE_INLINE void HideEditorWindow() NOEXCEPT
		{
			_Window.Hide();
		}

	private:

		//The window.
		Audio::PluginWindow _Window;

		//The renderer.
		Audio::PluginRenderer *RESTRICT _Renderer{ nullptr };

		//The UI system.
		UISystem _UISystem;

		//The plugin.
		Audio::Plugin *RESTRICT _Plugin{ nullptr };

		//The delta timer.
		DeltaTimer<float32> _DeltaTimer;

		/*
		*	Updates this plugin editor.
		*/
		void Update() NOEXCEPT;

	};

}
#endif