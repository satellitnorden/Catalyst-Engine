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
		*	Returns the clickable interface (if this widget is clickable.
		*/
		FORCE_INLINE NO_DISCARD ArrayProxy<UI::ClickableInterface *const RESTRICT> GetClickableInterfaces() NOEXCEPT override
		{
			static thread_local UI::ClickableInterface *RESTRICT CLICKABLE_INTERFACE_POINTER;
			CLICKABLE_INTERFACE_POINTER = &_ClickableInterface;
			return ArrayProxy<UI::ClickableInterface *const RESTRICT>(CLICKABLE_INTERFACE_POINTER);
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

		/*
		*	Resets the animation.
		*/
		void ResetAnimation() NOEXCEPT;

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