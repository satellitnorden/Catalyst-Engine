//Header file.
#include <UI/Widgets/SliderWidget.h>

//UI.
#include <UI/Core/Scene.h>

//STD.
#include <cstdio>

namespace UI
{

	//Slider widget constants.
	namespace SliderWidgetConstants
	{
		constexpr Vector4<float32> IDLE_COLOR{ 0.125f, 0.125f, 0.125f, 0.25f };
		constexpr Vector4<float32> HOVERED_COLOR{ 0.25f, 0.25f, 0.25f, 0.5f };
		constexpr Vector4<float32> PRESSED_COLOR{ 0.5f, 0.5f, 0.5f, 1.0f };

		constexpr float32 RADIUS{ 8.0f };
	}

	/*
	*	Default constructor.
	*/
	SliderWidget::SliderWidget() NOEXCEPT
	{
		//Set up the '_OnStateChangedCallback' on the clickable interface.
		_ClickableInterface._OnStateChanged = [](UI::Widget *const RESTRICT widget, UI::ClickableInterface *const RESTRICT clickable_interface, const UI::ClickableInterface::State previous_state, const UI::ClickableInterface::State new_state)
		{
			SliderWidget *const RESTRICT _this{ static_cast<SliderWidget *const RESTRICT>(widget) };

			switch (previous_state)
			{
				case UI::ClickableInterface::State::IDLE:
				{
					_this->_SourceColor = SliderWidgetConstants::IDLE_COLOR;

					break;
				}

				case UI::ClickableInterface::State::HOVERED:
				{
					_this->_SourceColor = SliderWidgetConstants::HOVERED_COLOR;

					break;
				}

				case UI::ClickableInterface::State::PRESSED:
				{
					_this->_SourceColor = SliderWidgetConstants::PRESSED_COLOR;

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

					_this->_DestinationColor = SliderWidgetConstants::IDLE_COLOR;

					_this->_AnimationDirection = AnimationDirection::LEFT;

					break;
				}

				case UI::ClickableInterface::State::HOVERED:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(2.0f);

					_this->_DestinationColor = SliderWidgetConstants::HOVERED_COLOR;

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

					_this->_DestinationColor = SliderWidgetConstants::PRESSED_COLOR;

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
	void SliderWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the text scale from the scene.
		_TextScale = _Parent->_Parent->GetTextScale();
	}

	/*
	*	Callback for when this widget is disabled.
	*/
	void SliderWidget::OnDisabled() NOEXCEPT
	{
		//Reset the animation.
		ResetAnimation();
	}

	/*
	*	Renders this widget.
	*/
	void SliderWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Update the value.
		if (_ClickableInterface.GetState() == UI::ClickableInterface::State::PRESSED)
		{
			const float32 alpha{ (_ClickableInterface.GetMousePosition()._X - _AxisAlignedBoundingBox._Minimum._X) / (_AxisAlignedBoundingBox._Maximum._X - _AxisAlignedBoundingBox._Minimum._X) };
			_Value = BaseMath::Round<int32>(BaseMath::LinearlyInterpolate(static_cast<float32>(_Minimum), static_cast<float32>(_Maximum), alpha));

			if (_OnValueChangedCallback)
			{
				_OnValueChangedCallback(GetParent()->_Parent, _Value);
			}
		}

		//Retrieve the current animator value.
		const float32 current_animator_value{ _Animator.Update(context._DeltaTime) };

		//Render the base box.
		RenderBox(context, _AxisAlignedBoundingBox, BaseMath::LinearlyInterpolate(_SourceColor, _DestinationColor, current_animator_value), Vector4<float32>(1.0f, 1.0f, 1.0f, 0.5f), SliderWidgetConstants::RADIUS);

		//Calculate the indicator box.
		AxisAlignedBoundingBox2D indicator_axis_aligned_bounding_box;

		{
			indicator_axis_aligned_bounding_box = _AxisAlignedBoundingBox;
			const float32 alpha{ static_cast<float32>(_Value - _Minimum) / static_cast<float32>(_Maximum - _Minimum) };
			indicator_axis_aligned_bounding_box._Maximum._X = BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._X, _AxisAlignedBoundingBox._Maximum._X, alpha);
		}

		//Render the indicator box.
		RenderBox(context, indicator_axis_aligned_bounding_box, BaseMath::LinearlyInterpolate(_SourceColor, _DestinationColor, current_animator_value) * 1.75f, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), SliderWidgetConstants::RADIUS);

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

			RenderBox(context, axis_aligned_bounding_box, color, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), SliderWidgetConstants::RADIUS);
		}

		//Render the text.
		{
			char buffer[64];
			sprintf_s(buffer, "%s%i%s", _Prefix ? _Prefix.Data() : "", _Value, _Postfix ? _Postfix.Data() : "");

			RenderText
			(
				context,
				_AxisAlignedBoundingBox,
				_Parent->_Parent->GetFont(),
				buffer,
				StringUtilities::StringLength(buffer),
				_TextScale,
				UI::HorizontalAlignment::CENTER,
				UI::VerticalAlignment::CENTER
			);
		}
	}

	/*
	*	Resets the animation.
	*/
	void SliderWidget::ResetAnimation() NOEXCEPT
	{
		//Set the animator's current value to 1.
		_Animator.SetCurrent(1.0f);

		//Set the source/destination color.
		_SourceColor = SliderWidgetConstants::IDLE_COLOR;
		_DestinationColor = SliderWidgetConstants::IDLE_COLOR;
	}

}