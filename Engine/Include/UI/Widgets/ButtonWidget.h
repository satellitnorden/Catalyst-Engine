#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//UI.
#include <UI/Core/Animator.h>
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Button widget.
	*/
	class ButtonWidget final : public UI::Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		ButtonWidget() NOEXCEPT;

		/*
		*	Callback for when the parent is available.
		*/
		void OnParentAvailable() NOEXCEPT override;

		/*
		*	Returns the clickable interface (if this widget is clickable.
		*/
		FORCE_INLINE NO_DISCARD UI::ClickableInterface *const RESTRICT GetClickableInterface() NOEXCEPT override
		{
			return &_ClickableInterface;
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

		//The source color.
		Vector4<float32> _SourceColor;

		//The destination color.
		Vector4<float32> _DestinationColor;

		//The animation direction.
		AnimationDirection _AnimationDirection{ AnimationDirection::RIGHT };

		//The text.
		DynamicString _Text;

		//The text scale.
		float32 _TextScale;

		/*
		*	Returns the button color.
		*/
		NO_DISCARD Vector4<float32> GetButtonColor() NOEXCEPT;

	};

}