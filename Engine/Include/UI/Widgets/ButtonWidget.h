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
	*	Button widget.
	*/
	class ButtonWidget final : public UI::Widget
	{

	public:

		//Type aliases.
		using OnPressedCallback = void(*)(UI::Scene *const RESTRICT scene, UI::Widget *const RESTRICT widget, void *const RESTRICT user_data);
		using OnStartHoverCallback = void(*)(UI::Scene *const RESTRICT scene, UI::Widget *const RESTRICT widget, void *const RESTRICT user_data);
		using OnEndHoverCallback = void(*)(UI::Scene *const RESTRICT scene, UI::Widget *const RESTRICT widget, void *const RESTRICT user_data);

		/*
		*	Default constructor.
		*/
		ButtonWidget() NOEXCEPT;

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
		*	Returns the text.
		*/
		FORCE_INLINE NO_DISCARD const DynamicString &GetText() const NOEXCEPT
		{
			return _Text;
		}

		/*
		*	Sets the text.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetText(const char *const RESTRICT text) NOEXCEPT
		{
			_Text = text;
			return this;
		}

		/*
		*	Sets the text scale.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetTextScale(const float32 value) NOEXCEPT
		{
			_TextScale = value;
			return this;
		}

		/*
		*	Sets the on start hover callback.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetOnStartHoverCallback(const OnStartHoverCallback callback) NOEXCEPT
		{
			_OnStartHoverCallback = callback;
			return this;
		}

		/*
		*	Sets the on start hover callback user data.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetOnStartHoverCallbackUserData(void *const RESTRICT user_data) NOEXCEPT
		{
			_OnStartHoverCallbackUserData = user_data;
			return this;
		}

		/*
		*	Sets the on end hover callback.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetOnEndHoverCallback(const OnStartHoverCallback callback) NOEXCEPT
		{
			_OnEndHoverCallback = callback;
			return this;
		}

		/*
		*	Sets the on end hover callback user data.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetOnEndHoverCallbackUserData(void *const RESTRICT user_data) NOEXCEPT
		{
			_OnEndHoverCallbackUserData = user_data;
			return this;
		}

		/*
		*	Sets the on pressed callback.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetOnPressedCallback(const OnPressedCallback callback) NOEXCEPT
		{
			_OnPressedCallback = callback;
			return this;
		}

		/*
		*	Sets the on pressed callback user data.
		*/
		FORCE_INLINE ButtonWidget *const RESTRICT SetOnPressedCallbackUserData(void *const RESTRICT user_data) NOEXCEPT
		{
			_OnPressedCallbackUserData = user_data;
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

		//The on start hover callback.
		OnStartHoverCallback _OnStartHoverCallback{ nullptr };

		//The on start hover callback user data.
		void *RESTRICT _OnStartHoverCallbackUserData{ nullptr };

		//The on end hover callback.
		OnEndHoverCallback _OnEndHoverCallback{ nullptr };

		//The on end hover callback user data.
		void *RESTRICT _OnEndHoverCallbackUserData{ nullptr };

		//The on pressed callback.
		OnPressedCallback _OnPressedCallback{ nullptr };

		//The on pressed callback user data.
		void *RESTRICT _OnPressedCallbackUserData{ nullptr };

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