#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//UI.
#include <UI/Core/Animator.h>
#include <UI/Core/Style.h>
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Slider widget.
	*/
	class SliderWidget final : public UI::Widget
	{

	public:

		//Type aliases.
		using OnValueChangedCallback = void(*)(UI::Scene *const RESTRICT scene, const int32 new_value);

		/*
		*	Default constructor.
		*/
		SliderWidget() NOEXCEPT;

		/*
		*	Callback for when the parent is available.
		*/
		void OnParentAvailable() NOEXCEPT override;

		/*
		*	Callback for when this widget is disabled.
		*/
		void OnDisabled() NOEXCEPT override;

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
		*	Sets the value.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetValue(const int32 value) NOEXCEPT
		{
			_Value = value;
			return this;
		}

		/*
		*	Sets the minimum.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetMinimum(const int32 minimum) NOEXCEPT
		{
			_Minimum = minimum;
			return this;
		}

		/*
		*	Sets the maximum.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetMaximum(const int32 maximum) NOEXCEPT
		{
			_Maximum = maximum;
			return this;
		}

		/*
		*	Sets the prefix.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetPrefix(const char *const RESTRICT prefix) NOEXCEPT
		{
			_Prefix = prefix;
			return this;
		}

		/*
		*	Sets the postfix.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetPostfix(const char *const RESTRICT postfix) NOEXCEPT
		{
			_Postfix = postfix;
			return this;
		}

		/*
		*	Sets the text scale.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetTextScale(const float32 value) NOEXCEPT
		{
			_TextScale = value;
			return this;
		}

		/*
		*	Sets the on value changed callback.
		*/
		FORCE_INLINE SliderWidget *const RESTRICT SetOnValueChangedCallback(const OnValueChangedCallback callback) NOEXCEPT
		{
			_OnValueChangedCallback = callback;
			return this;
		}

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

	private:

		//Enumeration covering all animation directions.
		enum class AnimationDirection : uint8
		{
			RIGHT,
			LEFT
		};

		//The value.
		int32 _Value{ 0 };

		//The minimum.
		int32 _Minimum{ INT32_MINIMUM };

		//The maximum.
		int32 _Maximum{ INT32_MAXIMUM };

		//The clickable interface.
		UI::ClickableInterface _ClickableInterface;

		//The animator.
		UI::Animator _Animator;

		//The source state.
		UI::ClickableInterface::State _SourceState;

		//The destination state.
		UI::ClickableInterface::State _DestinationState;

		//The animation direction.
		AnimationDirection _AnimationDirection{ AnimationDirection::RIGHT };

		//The prefix.
		DynamicString _Prefix;

		//The postfix.
		DynamicString _Postfix;

		//The style.
		UI::Style _Style;

		//The text scale.
		float32 _TextScale;

		//The on value changed callback.
		OnValueChangedCallback _OnValueChangedCallback{ nullptr };

		/*
		*	Resets the animation.
		*/
		void ResetAnimation() NOEXCEPT;

		/*
		*	Returns the color for the given state.
		*/
		NO_DISCARD Vector4<float32> GetColor(const UI::ClickableInterface::State state, const bool overlay) NOEXCEPT;

	};

}