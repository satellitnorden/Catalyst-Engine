//Header file.
#include <UI/Widgets/ButtonWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	ButtonWidget::ButtonWidget() NOEXCEPT
	{
		//Set up the '_OnStateChangedCallback' on the clickable interface.
		_ClickableInterface._OnStateChanged = [](UI::Widget *const RESTRICT widget, UI::ClickableInterface *const RESTRICT clickable_interface, const UI::ClickableInterface::State previous_state, const UI::ClickableInterface::State new_state)
		{
			ButtonWidget *const RESTRICT _this{ static_cast<ButtonWidget *const RESTRICT>(widget) };

			_this->_SourceState = previous_state;
			_this->_DestinationState = new_state;

			switch (new_state)
			{
				case UI::ClickableInterface::State::IDLE:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(2.0f);

					_this->_AnimationDirection = AnimationDirection::LEFT;

					if (previous_state == UI::ClickableInterface::State::HOVERED)
					{
						if (_this->_OnEndHoverCallback)
						{
							_this->_OnEndHoverCallback(_this->GetParent()->_Parent, _this, _this->_OnEndHoverCallbackUserData);
						}
					}

					break;
				}

				case UI::ClickableInterface::State::HOVERED:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(2.0f);

					if (previous_state == UI::ClickableInterface::State::IDLE)
					{
						_this->_AnimationDirection = AnimationDirection::RIGHT;
					}

					else
					{
						_this->_AnimationDirection = AnimationDirection::LEFT;
					}

					if (previous_state == UI::ClickableInterface::State::IDLE)
					{
						if (_this->_OnStartHoverCallback)
						{
							_this->_OnStartHoverCallback(_this->GetParent()->_Parent, _this, _this->_OnStartHoverCallbackUserData);
						}
					}

					else if (previous_state == UI::ClickableInterface::State::PRESSED)
					{
						if (_this->_OnPressedCallback)
						{
							_this->_OnPressedCallback(_this->GetParent()->_Parent, _this, _this->_OnPressedCallbackUserData);
						}
					}

					break;
				}

				case UI::ClickableInterface::State::PRESSED:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(16.0f);

					_this->_AnimationDirection = AnimationDirection::RIGHT;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		};

		//Reset the animation.
		ResetAnimation();
	}

	/*
	*	Callback for when the parent is available.
	*/
	void ButtonWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the style from the scene.
		_Style = _Parent->_Parent->GetStyle();

		//Grab the text scale from the scene.
		_TextScale = _Parent->_Parent->GetTextScale();
	}

	/*
	*	Callback for when this widget is disabled.
	*/
	void ButtonWidget::OnDisabled() NOEXCEPT
	{
		//Reset the animation.
		ResetAnimation();
	}

	/*
	*	Renders this widget.
	*/
	void ButtonWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Retrieve the current animator value.
		const float32 current_animator_value{ _Animator.Update(context._DeltaTime) };

		//Render the base box.
		RenderBox(context, _AxisAlignedBoundingBox, BaseMath::LinearlyInterpolate(GetColor(_SourceState), GetColor(_DestinationState), current_animator_value), Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Style._Rounding);

		//Draw the overlay box.
		if (current_animator_value < 1.0f)
		{
			AxisAlignedBoundingBox2D axis_aligned_bounding_box{ _AxisAlignedBoundingBox };
			Vector4<float32> color;

			switch (_AnimationDirection)
			{
				case AnimationDirection::RIGHT:
				{
					axis_aligned_bounding_box._Maximum._X = BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum._X, axis_aligned_bounding_box._Maximum._X, current_animator_value);
					color = GetColor(_DestinationState);

					break;
				}

				case AnimationDirection::LEFT:
				{
					axis_aligned_bounding_box._Minimum._X = BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Maximum._X, axis_aligned_bounding_box._Minimum._X, current_animator_value);
					color = GetColor(_SourceState);

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			color._A *= (1.0f - current_animator_value);

			RenderBox(context, axis_aligned_bounding_box, color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Style._Rounding);
		}

		//Render the text.
		if (_Text.Length() > 0)
		{
			RenderText
			(
				context,
				_AxisAlignedBoundingBox,
				_Parent->_Parent->GetFont(),
				_Text.Data(),
				_Text.Length(),
				_TextScale,
				UI::HorizontalAlignment::CENTER,
				UI::VerticalAlignment::CENTER
			);
		}
	}

	/*
	*	Resets the animation.
	*/
	void ButtonWidget::ResetAnimation() NOEXCEPT
	{
		//Set the animator's current value to 1.
		_Animator.SetCurrent(1.0f);

		//Set the source/destination states.
		_SourceState = _DestinationState = UI::ClickableInterface::State::IDLE;
	}

	/*
	*	Returns the color for the given state.
	*/
	NO_DISCARD Vector4<float32> ButtonWidget::GetColor(const UI::ClickableInterface::State state) NOEXCEPT
	{
		switch (state)
		{
			case UI::ClickableInterface::State::IDLE:
			{
				return _Style._UncheckedIdleColor;
			}

			case UI::ClickableInterface::State::HOVERED:
			{
				return _Style._UncheckedHoveredColor;
			}

			case UI::ClickableInterface::State::PRESSED:
			{
				return _Style._UncheckedPressedColor;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}

}