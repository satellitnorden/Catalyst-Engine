//Header file.
#include <UI/Widgets/SliderWidget.h>

//UI.
#include <UI/Core/Scene.h>

//STD.
#include <cstdio>

namespace UI
{

	/*
	*	Default constructor.
	*/
	SliderWidget::SliderWidget() NOEXCEPT
	{
		//Set up the '_OnStateChangedCallback' on the clickable interface.
		_ClickableInterface._OnStateChanged = [](UI::Widget *const RESTRICT widget, UI::ClickableInterface *const RESTRICT clickable_interface, const UI::ClickableInterface::State previous_state, const UI::ClickableInterface::State new_state)
		{
			SliderWidget *const RESTRICT _this{ static_cast<SliderWidget *const RESTRICT>(widget) };

			_this->_SourceState = previous_state;
			_this->_DestinationState = new_state;

			switch (new_state)
			{
				case UI::ClickableInterface::State::IDLE:
				{
					_this->_Animator.SetCurrent(0.0f);
					_this->_Animator.SetSpeed(2.0f);

					_this->_AnimationDirection = AnimationDirection::LEFT;

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
	void SliderWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the style from the scene.
		_Style = _Parent->_Parent->GetStyle();

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
		RenderBox(context, _AxisAlignedBoundingBox, BaseMath::LinearlyInterpolate(GetColor(_SourceState, false), GetColor(_DestinationState, false), current_animator_value), Vector4<float32>(1.0f, 1.0f, 1.0f, 0.5f), _Style._Rounding);

		//Calculate the indicator box.
		AxisAlignedBoundingBox2D indicator_axis_aligned_bounding_box;

		{
			indicator_axis_aligned_bounding_box = _AxisAlignedBoundingBox;
			const float32 alpha{ static_cast<float32>(_Value - _Minimum) / static_cast<float32>(_Maximum - _Minimum) };
			indicator_axis_aligned_bounding_box._Maximum._X = BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._X, _AxisAlignedBoundingBox._Maximum._X, alpha);
		}

		//Render the indicator box.
		RenderBox(context, indicator_axis_aligned_bounding_box, BaseMath::LinearlyInterpolate(GetColor(_SourceState, true), GetColor(_DestinationState, true), current_animator_value), Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Style._Rounding);

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
					color = GetColor(_DestinationState, false);

					break;
				}

				case AnimationDirection::LEFT:
				{
					axis_aligned_bounding_box._Minimum._X = BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Maximum._X, axis_aligned_bounding_box._Minimum._X, current_animator_value);
					color = GetColor(_SourceState, false);

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

		//Set the source/destination states.
		_SourceState = _DestinationState = UI::ClickableInterface::State::IDLE;
	}

	/*
	*	Returns the color for the given state.
	*/
	NO_DISCARD Vector4<float32> SliderWidget::GetColor(const UI::ClickableInterface::State state, const bool overlay) NOEXCEPT
	{
		if (overlay)
		{
			switch (state)
			{
				case UI::ClickableInterface::State::IDLE:
				{
					return _Style._CheckedIdleColor;
				}

				case UI::ClickableInterface::State::HOVERED:
				{
					return _Style._CheckedHoveredColor;
				}

				case UI::ClickableInterface::State::PRESSED:
				{
					return _Style._CheckedPressedColor;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					return Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}
		}

		else
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

}