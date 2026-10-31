#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Plugins/Core/Plugin.h>

//UI.
#include <UI/Core/Style.h>
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Switch widget.
	*/
	class SwitchWidget final : public UI::Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		SwitchWidget() NOEXCEPT;

		/*
		*	Callback for when the parent is available.
		*/
		void OnParentAvailable() NOEXCEPT override;

		/*
		*	Returns the number of clickable interfaces.
		*/
		FORCE_INLINE NO_DISCARD uint64 GetNumberOfClickableInterfaces() NOEXCEPT override
		{
			return 1;
		}

		/*
		*	Returns the clickable interface at the given index..
		*/
		FORCE_INLINE NO_DISCARD UI::ClickableInterface *const RESTRICT GetClickableInterface(const uint64 index) NOEXCEPT override
		{
			return &_ClickableInterface;
		}

		/*
		*	Sets an audio plugin/parameter index pair that this widget will pull data from.
		*/
		FORCE_INLINE NO_DISCARD SwitchWidget *const RESTRICT SetAudioPluginParameter(Audio::Plugin *const RESTRICT plugin, const uint64 parameter_index) NOEXCEPT
		{
			_AudioPlugin = plugin;
			_AudioPluginParameterIndex = parameter_index;

			return this;
		}

		/*
		*	Sets the texture indices.
		*/
		FORCE_INLINE NO_DISCARD SwitchWidget *const RESTRICT SetTextureIndices(const StaticArray<uint32, 2> &value) NOEXCEPT
		{
			_HasTextureIndices = true;
			_TextureIndices = value;

			return this;
		}

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

	private:

		//The clickable interface.
		UI::ClickableInterface _ClickableInterface;

		//The style.
		UI::Style _Style;

		//The audio plugin.
		Audio::Plugin *RESTRICT _AudioPlugin{ nullptr };

		//The audio plugin parameter index.
		uint64 _AudioPluginParameterIndex{ UINT64_MAXIMUM };

		//Denotes whether or not this widget has texture indices.
		bool _HasTextureIndices{ false };

		//The texture indices.
		StaticArray<uint32, 2> _TextureIndices;

		/*
		*	The on end press callback.
		*/
		void OnEndPress() NOEXCEPT;

		/*
		*	Returns the text bounding box.
		*/
		NO_DISCARD AxisAlignedBoundingBox2D GetTextBoundingBox() const NOEXCEPT;

		/*
		*	Returns the switch bounding box.
		*/
		NO_DISCARD AxisAlignedBoundingBox2D GetSwitchBoundingBox() const NOEXCEPT;

		/*
		*	Returns the off color for the given state.
		*/
		NO_DISCARD Vector4<float32> GetOffColor(const UI::ClickableInterface::State state) NOEXCEPT;

		/*
		*	Returns the on color for the given state.
		*/
		NO_DISCARD Vector4<float32> GetOnColor(const UI::ClickableInterface::State state) NOEXCEPT;

	};

}