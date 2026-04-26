//Header file.
#include <UI/Widgets/SelectorWidget.h>

//UI.
#include <UI/Core/Scene.h>

//STD.
#include <cstdio>

namespace UI
{

	/*
	*	Default constructor.
	*/
	SelectorWidget::SelectorWidget() NOEXCEPT
	{
		//Reset the animation.
		ResetAnimation();
	}

	/*
	*	Callback for when the parent is available.
	*/
	void SelectorWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the style from the scene.
		_Style = _Parent->_Parent->GetStyle();

		//Grab the text scale from the scene.
		_TextScale = _Parent->_Parent->GetTextScale();
	}

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void SelectorWidget::Position() NOEXCEPT
	{
		//Call the parent function.
		Widget::Position();

		//Set up the clickable interfaces.
		for (uint64 clickable_interface_index{ 0 }; clickable_interface_index < 2; ++clickable_interface_index)
		{
			ClickableInterfaceData &clickable_interface_data{ _ClickableInterfaceData[clickable_interface_index] };

			clickable_interface_data._AxisAlignedBoundingBox = _AxisAlignedBoundingBox;

			if (clickable_interface_index == 0)
			{
				clickable_interface_data._AxisAlignedBoundingBox._Maximum._X = clickable_interface_data._AxisAlignedBoundingBox._Minimum._X + (_AxisAlignedBoundingBox._Maximum._Y - _AxisAlignedBoundingBox._Minimum._Y);
			}

			else
			{
				clickable_interface_data._AxisAlignedBoundingBox._Minimum._X = clickable_interface_data._AxisAlignedBoundingBox._Maximum._X - (_AxisAlignedBoundingBox._Maximum._Y - _AxisAlignedBoundingBox._Minimum._Y);
			}

			clickable_interface_data._Index = clickable_interface_index;

			UI::ClickableInterface &clickable_interface{ _ClickableInterfaces[clickable_interface_index] };

			clickable_interface.SetUserData(&clickable_interface_data);

			clickable_interface._IsInside = [](const class Widget *const RESTRICT widget, ClickableInterface *const RESTRICT clickable_interface, const Vector2<float32> position)
			{
				const ClickableInterfaceData *const RESTRICT clickable_interface_data{ static_cast<const ClickableInterfaceData *const RESTRICT>(clickable_interface->GetUserData()) };
				return clickable_interface_data->_AxisAlignedBoundingBox.IsInside(position);
			};
			clickable_interface._OnStateChanged = [](UI::Widget *const RESTRICT widget, UI::ClickableInterface *const RESTRICT clickable_interface, const UI::ClickableInterface::State previous_state, const UI::ClickableInterface::State new_state)
			{
				SelectorWidget *const RESTRICT _this{ static_cast<SelectorWidget *const RESTRICT>(widget) };
				const ClickableInterfaceData *const RESTRICT clickable_interface_data{ static_cast<const ClickableInterfaceData *const RESTRICT>(clickable_interface->GetUserData()) };

				_this->_SourceStates[clickable_interface_data->_Index] = previous_state;
				_this->_DestinationStates[clickable_interface_data->_Index] = new_state;

				switch (new_state)
				{
					case UI::ClickableInterface::State::IDLE:
					{
						_this->_Animators[clickable_interface_data->_Index].SetCurrent(0.0f);
						_this->_Animators[clickable_interface_data->_Index].SetSpeed(2.0f);

						_this->_AnimationDirections[clickable_interface_data->_Index] = AnimationDirection::LEFT;

						break;
					}

					case UI::ClickableInterface::State::HOVERED:
					{
						_this->_Animators[clickable_interface_data->_Index].SetCurrent(0.0f);
						_this->_Animators[clickable_interface_data->_Index].SetSpeed(2.0f);

						if (previous_state == UI::ClickableInterface::State::IDLE)
						{
							_this->_AnimationDirections[clickable_interface_data->_Index] = AnimationDirection::RIGHT;
						}

						else
						{
							_this->_AnimationDirections[clickable_interface_data->_Index] = AnimationDirection::LEFT;
						}

						break;
					}

					case UI::ClickableInterface::State::PRESSED:
					{
						_this->_Animators[clickable_interface_data->_Index].SetCurrent(0.0f);
						_this->_Animators[clickable_interface_data->_Index].SetSpeed(16.0f);

						_this->_AnimationDirections[clickable_interface_data->_Index] = AnimationDirection::RIGHT;

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			};
			clickable_interface.SetOnEndPressCallback
			(
				clickable_interface_index == 0 ? 
				[](class Widget *const RESTRICT widget, void *const RESTRICT)
				{
					static_cast<SelectorWidget *const RESTRICT>(widget)->Decrement();
				}
				:
				[](class Widget *const RESTRICT widget, void *const RESTRICT)
				{
					static_cast<SelectorWidget *const RESTRICT>(widget)->Increment();
				}
			);

			_ClickableInterfacePointers[clickable_interface_index] = &clickable_interface;
		}
	}

	/*
	*	Callback for when this widget is disabled.
	*/
	void SelectorWidget::OnDisabled() NOEXCEPT
	{
		//Reset the animation.
		ResetAnimation();
	}

	/*
	*	Renders this widget.
	*/
	void SelectorWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Draw the clickable interface.
		for (uint64 clickable_interface_index{ 0 }; clickable_interface_index < 2; ++clickable_interface_index)
		{
			//Retrieve the current animator value.
			const float32 current_animator_value{ _Animators[clickable_interface_index].Update(context._DeltaTime)};

			//Render the base box.
			RenderBox(context, _ClickableInterfaceData[clickable_interface_index]._AxisAlignedBoundingBox, BaseMath::LinearlyInterpolate(GetColor(_SourceStates[clickable_interface_index]), GetColor(_DestinationStates[clickable_interface_index]), current_animator_value), Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Style._Rounding);

			//Draw the overlay box.
			if (current_animator_value < 1.0f)
			{
				AxisAlignedBoundingBox2D axis_aligned_bounding_box{ _ClickableInterfaceData[clickable_interface_index]._AxisAlignedBoundingBox };
				Vector4<float32> color;

				switch (_AnimationDirections[clickable_interface_index])
				{
					case AnimationDirection::RIGHT:
					{
						axis_aligned_bounding_box._Maximum._X = BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum._X, axis_aligned_bounding_box._Maximum._X, current_animator_value);
						color = GetColor(_DestinationStates[clickable_interface_index]);

						break;
					}

					case AnimationDirection::LEFT:
					{
						axis_aligned_bounding_box._Minimum._X = BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Maximum._X, axis_aligned_bounding_box._Minimum._X, current_animator_value);
						color = GetColor(_SourceStates[clickable_interface_index]);

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

			{
				AxisAlignedBoundingBox2D indicator_axis_aligned_bounding_box{ _ClickableInterfaceData[clickable_interface_index]._AxisAlignedBoundingBox };
				const float32 indicator_size{ (indicator_axis_aligned_bounding_box._Maximum._X - indicator_axis_aligned_bounding_box._Minimum._X) * 0.5f };
				indicator_axis_aligned_bounding_box._Minimum._X += indicator_size * 0.5f;
				indicator_axis_aligned_bounding_box._Minimum._Y += indicator_size * 0.5f;

				RenderText
				(
					context,
					indicator_axis_aligned_bounding_box,
					_Parent->_Parent->GetFont(),
					clickable_interface_index == 0 ? "<" : ">",
					1,
					indicator_size * (1.0f + 1.0f - 0.5f + 0.25f),
					UI::HorizontalAlignment::LEFT,
					UI::VerticalAlignment::BOTTOM
				);
			}
		}
		
		//Render the text.
		{
			char buffer[128];

			switch (_ValuesType)
			{
				case ValuesType::VALUES_INDEX:
				{
					sprintf_s(buffer, "%s%s%s", _Prefix ? _Prefix.Data() : "", _ValuesIndexData._Values[_ValuesIndexData._CurrentIndex].Data(), _Postfix ? _Postfix.Data() : "");

					break;
				}

				case ValuesType::MINIMUM_MAXIMUM:
				{
					sprintf_s(buffer, "%s%i%s", _Prefix ? _Prefix.Data() : "", _MinimumMaximumData._Current, _Postfix ? _Postfix.Data() : "");

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			AxisAlignedBoundingBox2D text_axis_aligned_bounding_box{ _AxisAlignedBoundingBox };

			text_axis_aligned_bounding_box._Minimum._X += (_AxisAlignedBoundingBox._Maximum._Y - _AxisAlignedBoundingBox._Minimum._Y);
			text_axis_aligned_bounding_box._Maximum._X -= (_AxisAlignedBoundingBox._Maximum._Y - _AxisAlignedBoundingBox._Minimum._Y);

			RenderText
			(
				context,
				text_axis_aligned_bounding_box,
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
	void SelectorWidget::ResetAnimation() NOEXCEPT
	{
		//Set the animator's current value to 1.
		for (UI::Animator &animator : _Animators) animator.SetCurrent(1.0f);

		//Set the source/destination states.
		for (UI::ClickableInterface::State &source_state : _SourceStates) source_state = UI::ClickableInterface::State::IDLE;
		for (UI::ClickableInterface::State &destination_state : _DestinationStates) destination_state = UI::ClickableInterface::State::IDLE;
	}

	/*
	*	Returns the color for the given state.
	*/
	NO_DISCARD Vector4<float32> SelectorWidget::GetColor(const UI::ClickableInterface::State state) NOEXCEPT
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

	/*
	*	Decrements.
	*/
	void SelectorWidget::Decrement() NOEXCEPT
	{
		switch (_ValuesType)
		{
			case ValuesType::VALUES_INDEX:
			{
				if (_ValuesIndexData._CurrentIndex == 0)
				{
					_ValuesIndexData._CurrentIndex = _ValuesIndexData._Values.LastIndex();
				}

				else
				{
					--_ValuesIndexData._CurrentIndex;
				}

				if (_OnIndexChangedCallback)
				{
					_OnIndexChangedCallback(GetParent()->_Parent, this, _OnIndexChangedCallbackUserData, _ValuesIndexData._CurrentIndex);
				}

				break;
			}

			case ValuesType::MINIMUM_MAXIMUM:
			{
				if (_MinimumMaximumData._Current == _MinimumMaximumData._Minimum)
				{
					_MinimumMaximumData._Current = _MinimumMaximumData._Maximum;
				}

				else
				{
					--_MinimumMaximumData._Current;
				}

				if (_OnValueChangedCallback)
				{
					_OnValueChangedCallback(GetParent()->_Parent, this, _OnValueChangedCallbackUserData, _MinimumMaximumData._Current);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Increments.
	*/
	void SelectorWidget::Increment() NOEXCEPT
	{
		switch (_ValuesType)
		{
			case ValuesType::VALUES_INDEX:
			{
				if (_ValuesIndexData._CurrentIndex == _ValuesIndexData._Values.LastIndex())
				{
					_ValuesIndexData._CurrentIndex = 0;
				}

				else
				{
					++_ValuesIndexData._CurrentIndex;
				}

				if (_OnIndexChangedCallback)
				{
					_OnIndexChangedCallback(GetParent()->_Parent, this, _OnIndexChangedCallbackUserData, _ValuesIndexData._CurrentIndex);
				}

				break;
			}

			case ValuesType::MINIMUM_MAXIMUM:
			{
				if (_MinimumMaximumData._Current == _MinimumMaximumData._Maximum)
				{
					_MinimumMaximumData._Current = _MinimumMaximumData._Minimum;
				}

				else
				{
					++_MinimumMaximumData._Current;
				}

				if (_OnValueChangedCallback)
				{
					_OnValueChangedCallback(GetParent()->_Parent, this, _OnValueChangedCallbackUserData, _MinimumMaximumData._Current);
				}

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

}