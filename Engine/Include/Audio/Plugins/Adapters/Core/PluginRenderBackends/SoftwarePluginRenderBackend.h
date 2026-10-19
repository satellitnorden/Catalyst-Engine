#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Adapters/Core/PluginRenderBackends/PluginRenderBackend.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox2D.h>

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
		return _Buffer.Data();
	}
	 
	/*
	*	Updates this render backend.
	*/
	void Update(const PluginUpdateContext &context, const DynamicArray<UI::RenderCommand> &render_commands) NOEXCEPT override;

private:

	//The buffer.
	Texture2D<uint32> _Buffer;

	/*
	*	Rasterizes the given render command.
	*/
	void Rasterize(const UI::RenderCommand &render_command) NOEXCEPT;

	/*
	*	The fragment function.
	*/
	NO_DISCARD Vector4<float32> Fragment
	(
		const UI::RenderCommand &render_command,
		const AxisAlignedBoundingBox2D &bounding_box,
		const Vector2<float32> pixel_position,
		const Vector2<float32> texture_coordinate
	) NOEXCEPT;

};