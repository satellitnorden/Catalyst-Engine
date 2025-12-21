//Header file.
#include <UI/Widgets/ButtonWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	//Button widget constants.
	namespace ButtonWidgetConstants
	{
		constexpr Vector4<float32> IDLE_COLOR{ 0.125f, 0.125f, 0.125f, 0.25f };
		constexpr Vector4<float32> HOVERED_COLOR{ 0.25f, 0.25f, 0.25f, 0.5f };
		constexpr Vector4<float32> PRESSED_COLOR{ 0.5f, 0.5f, 0.5f, 1.0f };

		constexpr float32 RADIUS{ 8.0f };
	}

	/*
	*	Default constructor.
	*/
	ButtonWidget::ButtonWidget() NOEXCEPT
	{
		//Set up the '_OnStateChangedCallback' on the clickable interface.
		_ClickableInterface._OnStateChanged = [](UI::Widget *const RESTRICT widget, const UI::ClickableInterface::State previous_state, const UI::ClickableInterface::State new_state)
		{
			ButtonWidget *const RESTRICT _this{ static_cast<ButtonWidget *const RESTRICT>(widget) };

			switch (previous_state)
			{
				case UI::ClickableInterface::State::IDLE:
				{
					_this->_SourceColor = ButtonWidgetConstants::IDLE_COLOR;

					break;
				}

				case UI::ClickableInterface::State::HOVERED:
				{
					_this->_SourceColor = ButtonWidgetConstants::HOVERED_COLOR;

					break;
				}

				case UI::ClickableInterface::State::PRESSED:
				{
					_this->_SourceColor = ButtonWidgetConstants::PRESSED_COLOR;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			switch (new_state)
			{
				case UI::ClickableInterface::State::IDLE:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(2.0f);

					_this->_DestinationColor = ButtonWidgetConstants::IDLE_COLOR;

					_this->_AnimationDirection = AnimationDirection::LEFT;

					break;
				}

				case UI::ClickableInterface::State::HOVERED:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(2.0f);

					_this->_DestinationColor = ButtonWidgetConstants::HOVERED_COLOR;

					if (previous_state == UI::ClickableInterface::State::IDLE)
					{
						_this->_AnimationDirection = AnimationDirection::RIGHT;
					}

					else
					{
						_this->_AnimationDirection = AnimationDirection::LEFT;
					}

					break;
				}

				case UI::ClickableInterface::State::PRESSED:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(16.0f);

					_this->_DestinationColor = ButtonWidgetConstants::PRESSED_COLOR;

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

		//Set the animator's current value to 1.
		_Animator.SetCurrent(1.0f);

		//Set the source/destination color.
		_SourceColor = ButtonWidgetConstants::IDLE_COLOR;
		_DestinationColor = ButtonWidgetConstants::IDLE_COLOR;
	}

	/*
	*	Callback for when the parent is available.
	*/
	void ButtonWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the text scale from the scene.
		_TextScale = _Parent->_Parent->GetTextScale();
	}

	/*
	*	Renders this widget.
	*/
	void ButtonWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Retrieve the current animator value.
		const float32 current_animator_value{ _Animator.Update(context._DeltaTime) };

		//Render the base box.
		RenderBox(context, _AxisAlignedBoundingBox, BaseMath::LinearlyInterpolate(_SourceColor, _DestinationColor, current_animator_value), Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), ButtonWidgetConstants::RADIUS);

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
					color = _DestinationColor;

					break;
				}

				case AnimationDirection::LEFT:
				{
					axis_aligned_bounding_box._Minimum._X = BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Maximum._X, axis_aligned_bounding_box._Minimum._X, current_animator_value);
					color = _SourceColor;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			color._A *= (1.0f - current_animator_value);

			RenderBox(context, axis_aligned_bounding_box, color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), ButtonWidgetConstants::RADIUS);
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
	*	Returns the button color.
	*/
	NO_DISCARD Vector4<float32> ButtonWidget::GetButtonColor() NOEXCEPT
	{
		switch (_ClickableInterface.GetState())
		{
			case UI::ClickableInterface::State::IDLE:
			{
				return ButtonWidgetConstants::IDLE_COLOR;
			}

			case UI::ClickableInterface::State::HOVERED:
			{
				return ButtonWidgetConstants::HOVERED_COLOR;
			}

			case UI::ClickableInterface::State::PRESSED:
			{
				return ButtonWidgetConstants::PRESSED_COLOR;
			}

			default:
			{
				ASSERT(false, "Invalid case!");
				return Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}

}