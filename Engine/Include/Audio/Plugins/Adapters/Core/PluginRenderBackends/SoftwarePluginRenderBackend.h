#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/PluginRenderBackend.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

/*
*	Software plugin render backend - Does software rendering. (:
*/
class SoftwarePluginRenderBackend final : public PluginRenderBackend
{

public:

	/*
	*	Default constructor.
	*/
	SoftwarePluginRenderBackend(const Audio::Plugin *const RESTRICT plugin) NOEXCEPT;

	/*
	*	Returns if this render backend is doing software rendering.
	*/
	NO_DISCARD bool IsDoingSoftwareRendering() const NOEXCEPT override
	{
		return true;
	}

	/*
	*	Returns the software render data.
	*/
	NO_DISCARD const uint32 *const RESTRICT GetSoftwareRenderData() const NOEXCEPT override
	{
		return _Data.Data();
	}

	/*
	*	Updates this render backend.
	*/
	void Update() NOEXCEPT override;

private:

	//The data.
	Texture2D<uint32> _Data;

};