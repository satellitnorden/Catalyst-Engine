#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//Forward declarations.
class PluginRenderer;

/*
*	Class holding a (child) window for a plugin.
*	Handles creation/destruction/showing/hiding as well as setting up update functions and handling window messages.
*/
class PluginWindow final
{

public:

	/*
	*	Initializes this plugin window.
	*/
	void Initialize(PluginRenderer *const RESTRICT renderer) NOEXCEPT;

	/*
	*	Creates this plugin window.
	*	Returns if the creation succeeded.
	*/
	NO_DISCARD bool Create(void *const RESTRICT parent_window, const Resolution resolution) NOEXCEPT;

	/*
	*	Shows this plugin window.
	*/
	void Show();

	/*
	*	Hides this plugin window.
	*/
	void Hide();

	/*
	*	Destroys this plugin window.
	*/
	void Destroy();

	/*
	*	Returns the renderer.
	*/
	FORCE_INLINE NO_DISCARD PluginRenderer *const RESTRICT GetRenderer() const NOEXCEPT
	{
		return _Renderer;
	}

	/*
	*	Returns the resolution.
	*/
	FORCE_INLINE NO_DISCARD Resolution GetResolution() const NOEXCEPT
	{
		return _Resolution;
	}

private:

	//The renderer.
	PluginRenderer *RESTRICT _Renderer;

	//The resolution.
	Resolution _Resolution;

	//The window.
	void *RESTRICT _Window{ nullptr };

};