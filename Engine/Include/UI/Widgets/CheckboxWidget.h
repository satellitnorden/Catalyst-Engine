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
	*	Checkbox widget.
	*/
	class CheckboxWidget final : public UI::Widget
	{

	public:

		//Type aliases.
		using OnValueChangedCallback = void(*)(UI::Scene *const RESTRICT scene, const bool new_value);

		/*
		*	Default constructor.
		*/
		CheckboxWidget() NOEXCEPT;

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
		FORCE_INLINE CheckboxWidget *const RESTRICT SetValue(const bool value) NOEXCEPT
		{
			_Value = value;
			return this;
		}

		/*
		*	Sets the text.
		*/
		FORCE_INLINE CheckboxWidget *const RESTRICT SetText(const char *const RESTRICT text) NOEXCEPT
		{
			_Text = text;
			return this;
		}

		/*
		*	Sets the text scale.
		*/
		FORCE_INLINE CheckboxWidget *const RESTRICT SetTextScale(const float32 value) NOEXCEPT
		{
			_TextScale = value;
			return this;
		}

		/*
		*	Sets the on value changed callback.
		*/
		FORCE_INLINE CheckboxWidget *const RESTRICT SetOnValueChangedCallback(const OnValueChangedCallback callback) NOEXCEPT
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
		bool _Value{ false };

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

		//The text.
		DynamicString _Text;

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
		NO_DISCARD Vector4<float32> GetColor(const UI::ClickableInterface::State state) NOEXCEPT;

	};

}