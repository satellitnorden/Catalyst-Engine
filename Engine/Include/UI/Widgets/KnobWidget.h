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
	*	Knob widget.
	*/
	class KnobWidget final : public UI::Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		KnobWidget() NOEXCEPT;

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
		FORCE_INLINE NO_DISCARD KnobWidget *const RESTRICT SetAudioPluginParameter(Audio::Plugin *const RESTRICT plugin, const uint64 parameter_index) NOEXCEPT
		{
			_AudioPlugin = plugin;
			_AudioPluginParameterIndex = parameter_index;

			return this;
		}

		/*
		*	Sets the texture indices.
		*/
		FORCE_INLINE NO_DISCARD KnobWidget *const RESTRICT SetTextureIndices(const DynamicArray<uint32> &value) NOEXCEPT
		{
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

		//The accumulated change in the value. This widget will commit the accumulated change at each "step".
		float32 _AccumulatedChange{ 0.0f };

		//The texture indices.
		DynamicArray<uint32> _TextureIndices;

		/*
		*	Returns the text bounding box.
		*/
		NO_DISCARD AxisAlignedBoundingBox2D GetTextBoundingBox() const NOEXCEPT;

		/*
		*	Returns the knob bounding box.
		*/
		NO_DISCARD AxisAlignedBoundingBox2D GetKnobBoundingBox() const NOEXCEPT;

		/*
		*	Returns the inner color for the given state.
		*/
		NO_DISCARD Vector4<float32> GetInnerColor(const UI::ClickableInterface::State state) NOEXCEPT;

		/*
		*	Returns the outer color for the given state.
		*/
		NO_DISCARD Vector4<float32> GetOuterColor(const UI::ClickableInterface::State state) NOEXCEPT;

	};

}