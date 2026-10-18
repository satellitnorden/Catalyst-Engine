//Header file.
#include <Audio/Plugins/Adapters/Core/PluginRenderer.h>

//Audio.
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/SoftwarePluginRenderBackend.h>

/*
*	Initializes this plugin renderer.
*/
void PluginRenderer::Initialize(Audio::Plugin *const RESTRICT plugin, const PluginWindow *const RESTRICT window) NOEXCEPT
{
	//Set the plugin.
	_Plugin = plugin;

	//Set the window.
	_Window = window;

	//Create the render backend.
	_RenderBackend = new SoftwarePluginRenderBackend(_Plugin);

	//Set up the UI system.
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

		context._UISystem = &_UISystem;

		_Plugin->Initialize(context);
	}
}

/*
*	Updates this plugin renderer.
*/
void PluginRenderer::Update(const PluginUpdateContext &context) NOEXCEPT
{
	//Update the UI system.
	_UISystem.Update(context._DeltaTime, context._MouseState);

	//Update the render backend.
	_RenderBackend->Update(context, _UISystem.GetRenderCommands());
}

/*
*	Returns if this renderer is doing software rendering.
*/
NO_DISCARD bool PluginRenderer::IsDoingSoftwareRendering() const NOEXCEPT
{
	return _RenderBackend->IsDoingSoftwareRendering();
}

/*
*	Returns the software render data.
*/
NO_DISCARD const uint32 *const RESTRICT PluginRenderer::GetSoftwareRenderData() const NOEXCEPT
{
	return _RenderBackend->GetSoftwareRenderData();
}