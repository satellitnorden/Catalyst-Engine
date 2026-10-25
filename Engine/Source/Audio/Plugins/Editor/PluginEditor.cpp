#if defined(AUDIO_PLUGIN)
//Header file.
#include <Audio/Plugins/Editor/PluginEditor.h>

//Audio.
#include <Audio/Plugins/Editor/Renderers/OpenGLPluginRenderer.h>
#include <Audio/Plugins/Editor/Renderers/SoftwarePluginRenderer.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/ContentSystem.h>

namespace Audio
{

	/*
	*	Initializes this plugin editor.
	*/
	void PluginEditor::Initialize(Audio::Plugin *const RESTRICT plugin) NOEXCEPT
	{
		//Initialize the content system.
		ContentSystem::Instance->Initialize();

		//Load the engine base assets.
		ContentSystem::Instance->LoadAssetCollection("Base.cac");

		//Set the plugin.
		_Plugin = plugin;

		//Initialize the UI system.
		_UISystem.SetReferenceResolution
		(
			Vector2<float32>
			(
				static_cast<float32>(_Plugin->GetResolution()._Width),
				static_cast<float32>(_Plugin->GetResolution()._Height)
			)
		);

		//Initialize the plugin.
		{
			Audio::Plugin::InitializeContext context;

			if (USE_INTERNAL_RENDERING)
			{
				context._UISystem = &_UISystem;
			}

			else
			{
				context._UISystem = UISystem::Instance;
			}

			_Plugin->CallInitialize(context);
		}
	}

	/*
	*	Creates the editor.
	*/
	void PluginEditor::CreateEditor(void *const RESTRICT parent_window) NOEXCEPT
	{
		//Only do all of this if we're using internal rendering.
		if (USE_INTERNAL_RENDERING)
		{
			//Set the update function on the window.
			_Window.SetUpdateFunction
			(
				[](void* const RESTRICT arguments)
				{
					static_cast<PluginEditor* const RESTRICT>(arguments)->Update();
				},
				this
			);

			//Initialize the window.
			_Window.Initialize(_Plugin->GetResolution(), parent_window);

			//Immediately show the window in standalone builds.
#if defined(AUDIO_PLUGIN_STANDALONE)
			_Window.Show();
#endif

			//Create the renderer.
			if (OpenGLPluginRenderer::IsSupported(&_Window))
			{
				_Renderer = new OpenGLPluginRenderer();
			}
			
			else if (SoftwarePluginRenderer::IsSupported(&_Window))
			{
				_Renderer = new SoftwarePluginRenderer();
			}

			else
			{
				ASSERT(false, "Could not create a renderer!");
				_Renderer = nullptr;
			}

			//Initialize the renderer.
			_Renderer->Initialize(_Plugin->GetResolution(), &_Window);
		}
	}

	/*
	*	Destroys the editor.
	*/
	void PluginEditor::DestroyEditor() NOEXCEPT
	{
		if (USE_INTERNAL_RENDERING)
		{
			//Destroy the renderer.
			if (_Renderer)
			{
				delete _Renderer;

				_Renderer = nullptr;
			}

			//Destroy the window.
			_Window.Destroy();
		}
	}

	/*
	*	Updates this plugin editor.
	*/
	void PluginEditor::Update() NOEXCEPT
	{
		PROFILING_SCOPE("PluginEditor::Update()");

		//Update the UI system.
		_UISystem.Update(_DeltaTimer.Update(), _Window.GetMouseState());

		//Render!
		_Renderer->Render(_UISystem.GetRenderCommands());
	}

}
#endif