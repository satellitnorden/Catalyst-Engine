#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Core/Plugin.h>

/*
*	Plugin render backend class - Responsible for doing the actual drawing, based on the given state.
*/
class PluginRenderBackend
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PluginRenderBackend(const Audio::Plugin *const RESTRICT plugin) NOEXCEPT
		:
		_Plugin(plugin)
	{

	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE virtual ~PluginRenderBackend() NOEXCEPT
	{

	}

	/*
	*	Returns if this render backend is doing software rendering.
	*/
	virtual NO_DISCARD bool IsDoingSoftwareRendering() const NOEXCEPT = 0;

	/*
	*	Returns the software render data.
	*/
	virtual NO_DISCARD const uint32 *const RESTRICT GetSoftwareRenderData() const NOEXCEPT = 0;

	/*
	*	Updates this render backend.
	*/
	virtual void Update() NOEXCEPT = 0;

protected:

	//The plugin.
	const Audio::Plugin *RESTRICT _Plugin;

};