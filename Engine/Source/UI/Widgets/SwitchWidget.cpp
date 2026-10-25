//Header file.
#include <UI/Widgets/SwitchWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Switch widget constants.
	*/
	namespace SwitchWidgetConstants
	{
		//Define constants.
		constexpr float32 TEXT_FRACTION{ 0.1875f }; //For each text - So times 2 for both the above and below text.
	}

	/*
	*	Default constructor.
	*/
	SwitchWidget::SwitchWidget() NOEXCEPT
	{
		//Set up the clickable interface.
		/*
		_ClickableInterface._IsInside = [](const Widget *const RESTRICT widget, ClickableInterface *const RESTRICT clickable_interface, const Vector2<float32> position)
		{
			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(widget->GetAxisAlignedBoundingBox()) };

			//Calculate the height.
			const float32 height{ widget->GetAxisAlignedBoundingBox().LongestSide() };

			//Calculate the outer circle radius, and it's square.
			const float32 outer_circle_radius{ height * 0.5f * (1.0f - (KnobWidgetConstants::TEXT_FRACTION * 2.0f)) };
			const float32 outer_circle_radius_squared{ outer_circle_radius * outer_circle_radius };

			//Calculate the distance to the center.
			const float32 distance_to_center{ Vector2<float32>::LengthSquared(center - position) };

			//The mouse is inside if it is inside the knob circle.
			return distance_to_center <= outer_circle_radius_squared;
		};
		*/
		_ClickableInterface.SetOnEndPressCallback
		(
			[](Widget *const RESTRICT widget, void *const RESTRICT /*user_data*/)
			{
				static_cast<SwitchWidget *const RESTRICT>(widget)->OnEndPress();
			}
		);
	}

	/*
	*	Callback for when the parent is available.
	*/
	void SwitchWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the style from the scene.
		_Style = _Parent->_Parent->GetStyle();
	}

	/*
	*	Renders this widget.
	*/
	void SwitchWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Calculate the center.
		const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(_AxisAlignedBoundingBox) };

		//Calculate the height.
		const float32 height{ _AxisAlignedBoundingBox.LongestSide() };

		//Render the indicator.
		{
			//Calculate the indicator height.
			const float32 indicator_height{ height * (1.0f - (SwitchWidgetConstants::TEXT_FRACTION * 2.0f)) };

			//Calculate the indicator width.
			const float32 indicator_width{ indicator_height * 0.5f };

			//Render the base box.
			{
				//Calculate the bounding box.
				const AxisAlignedBoundingBox2D bounding_box
				{
					Vector2<float32>(center._X - indicator_width * 0.5f, center._Y - indicator_height * 0.5f),
					Vector2<float32>(center._X + indicator_width * 0.5f, center._Y + indicator_height * 0.5f)
				};

				//Render the box.
				RenderRoundedBox(context, bounding_box, GetOffColor(_ClickableInterface.GetState()), Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Style._Rounding);
			}

			//Render the indicator box.
			{
				//Cache the switch state.
				const bool switch_state{ _AudioPlugin->GetParameterCurrentPlain(_AudioPluginParameterIndex) >= 0.5f };

				//Calculate the bounding box.
				const AxisAlignedBoundingBox2D bounding_box
				{
					Vector2<float32>(center._X - indicator_width * 0.5f, center._Y - indicator_height * 0.5f * static_cast<float32>(!switch_state)),
					Vector2<float32>(center._X + indicator_width * 0.5f, center._Y + indicator_height * 0.5f * static_cast<float32>(switch_state))
				};

				//Render the box.
				RenderRoundedBox(context, bounding_box, GetOnColor(_ClickableInterface.GetState()), Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Style._Rounding);
			}
		}

		//Render the text above.
		{
			//Figure out the text.
			const char *RESTRICT text{ nullptr };

			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				text = _AudioPlugin->GetParameterDisplayName(_AudioPluginParameterIndex);
			}

			if (text)
			{
				AxisAlignedBoundingBox2D bounding_box{ _AxisAlignedBoundingBox };
				bounding_box._Minimum._Y = bounding_box._Maximum._Y - height * SwitchWidgetConstants::TEXT_FRACTION;

				RenderText
				(
					context,
					bounding_box,
					_Parent->_Parent->GetFont(),
					text,
					bounding_box.Dimensions()._Y
				);
			}
		}

		//Render the text below.
		{
			//Figure out the text.
			char buffer[64];

			const char *RESTRICT text{ nullptr };

			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				_AudioPlugin->PrintParameterValueString(_AudioPluginParameterIndex, buffer, ARRAY_LENGTH(buffer), _AudioPlugin->GetParameterCurrentPlain(_AudioPluginParameterIndex));
				text = buffer;
			}

			if (text)
			{
				AxisAlignedBoundingBox2D bounding_box{ _AxisAlignedBoundingBox };
				bounding_box._Maximum._Y = bounding_box._Minimum._Y + height * SwitchWidgetConstants::TEXT_FRACTION;

				RenderText
				(
					context,
					bounding_box,
					_Parent->_Parent->GetFont(),
					text,
					bounding_box.Dimensions()._Y
				);
			}
		}
	}

	/*
	*	The on end press callback.
	*/
	void SwitchWidget::OnEndPress() NOEXCEPT
	{
		//Cache the switch state.
		const bool switch_state{ _AudioPlugin->GetParameterCurrentPlain(_AudioPluginParameterIndex) >= 0.5f };

		//Set the parameter.
		_AudioPlugin->SetParameterPlain(_AudioPluginParameterIndex, static_cast<float32>(!switch_state));
	}

	/*
	*	Returns the off color for the given state.
	*/
	NO_DISCARD Vector4<float32> SwitchWidget::GetOffColor(const UI::ClickableInterface::State state) NOEXCEPT
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
	*	Returns the on color for the given state.
	*/
	NO_DISCARD Vector4<float32> SwitchWidget::GetOnColor(const UI::ClickableInterface::State state) NOEXCEPT
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

}