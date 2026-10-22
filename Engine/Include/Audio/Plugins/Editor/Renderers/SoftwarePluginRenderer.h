#if defined(AUDIO_PLUGIN)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/HashTable.h>

//Audio.
#include <Audio/Plugins/Editor/PluginRenderer.h>

//Content.
#include <Content/Assets/FontAsset.h>

namespace Audio
{

	class SoftwarePluginRenderer final : public PluginRenderer
	{

	public:

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

		//The window.
		Audio::PluginWindow *RESTRICT _Window;

		//The texture index to font asset lookup.
		HashTable<uint32, const FontAsset *RESTRICT> _TextureIndexToFontAssetLookup;

		//The buffer.
		Texture2D<uint32> _Buffer;

		//The last rendered command buffer..
		Texture2D<uint32> _LastRenderedCommandBuffer;

		/*
		*	Builds the texture index to font asset lookup.
		*/
		void BuildTextureIndexToFontAssetLookup() NOEXCEPT;

		/*
		*	Unpacks a color.
		*/
		NO_DISCARD Vector4<float32> UnpackColor(const uint32 color) NOEXCEPT;

		/*
		*	Packs a color.
		*/
		NO_DISCARD uint32 PackColor(const Vector4<float32>& color) NOEXCEPT;

		/*
		*	Rasterizes the given render command.
		*/
		void Rasterize(const uint32 render_command_index, const UI::RenderCommand &render_command) NOEXCEPT;

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

}
#endif