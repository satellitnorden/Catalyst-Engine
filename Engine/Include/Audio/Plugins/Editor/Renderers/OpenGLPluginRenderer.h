#if defined(AUDIO_PLUGIN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>

//Audio.
#include <Audio/Plugins/Editor/PluginRenderer.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

namespace Audio
{

	class OpenGLPluginRenderer final : public PluginRenderer
	{

	public:

		/*
		*	Returns if this plugin renderer is supported.
		*/
		FORCE_INLINE static NO_DISCARD bool IsSupported(const Audio::PluginWindow *const RESTRICT window) NOEXCEPT
		{
			//This is supported if the window supports OpenGL.
			return window->GetSupportsOpenGL();
		}

		/*
		*	Initializes this plugin renderer.
		*/
		void Initialize(const Resolution resolution, Audio::PluginWindow *const RESTRICT window) NOEXCEPT override;

		/*
		*	Renders.
		*/
		void Render(const DynamicArray<UI::RenderCommand> &render_commands) NOEXCEPT override;

		/*
		*	Terminates this plugin renderer.
		*/
		void Terminate() NOEXCEPT override;

	private:

		//The resolution.
		Resolution _Resolution;

		//The window.
		Audio::PluginWindow *RESTRICT _Window;

		//The program.
		OpenGL::Identifier _Program;

		//The vertex array object.
		OpenGL::Identifier _VertexArrayObject;

		//The textures.
		HashTable<uint32, OpenGL::Identifier> _Textures;

		/*
		*	Creates the textures.
		*/
		void CreateTextures() NOEXCEPT;

		/*
		*	Renders the given render command.
		*/
		void Render(const UI::RenderCommand &render_command) NOEXCEPT;

	};

}
#endif