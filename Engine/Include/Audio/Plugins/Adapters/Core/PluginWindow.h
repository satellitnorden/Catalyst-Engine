#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

/*
*	Class holding a (child) window for a plugin.
*	Handles creation/destruction/showing/hiding as well as setting up update functions and handling window messages.
*/
class PluginWindow final
{

public:

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

private:

	//The window.
	void *RESTRICT _Window{ nullptr };

};