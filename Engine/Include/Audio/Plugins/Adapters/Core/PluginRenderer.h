#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Core/Plugin.h>
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/PluginRenderBackend.h>
#include <Audio/Plugins/Adapters/Core/PluginUpdateContext.h>

//Systems.
#include <Systems/UISystem.h>

//Forward declarations.
class PluginWindow;

/*
*	Plugin renderer class - Has the responsibility of gathering the input state from the plugin window, updating the UI scene(s) of the plugin and rendering it.
*	The plugin renderer takes responsibility for deciding how rendering should be done (which backend to choose).
*/
class PluginRenderer final
{

public:

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~PluginRenderer() NOEXCEPT
	{
		//Destroy the render backend.
		delete _RenderBackend;
	}

	/*
	*	Initializes this plugin renderer.
	*/
	void Initialize(Audio::Plugin *const RESTRICT plugin, const PluginWindow *const RESTRICT window) NOEXCEPT;

	/*
	*	Updates this plugin renderer.
	*/
	void Update(const PluginUpdateContext &context) NOEXCEPT;

	/*
	*	Returns if this renderer is doing software rendering.
	*/
	NO_DISCARD bool IsDoingSoftwareRendering() const NOEXCEPT;

	/*
	*	Returns the software render data.
	*/
	NO_DISCARD const uint32 *const RESTRICT GetSoftwareRenderData() const NOEXCEPT;

private:

	//The plugin.
	Audio::Plugin *RESTRICT _Plugin;

	//The window.
	const PluginWindow *RESTRICT _Window;

	//The render backend.
	PluginRenderBackend *RESTRICT _RenderBackend;

	//The UI System.
	UISystem _UISystem;

};