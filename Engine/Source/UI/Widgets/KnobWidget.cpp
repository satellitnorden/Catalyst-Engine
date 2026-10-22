//Header file.
#include <UI/Widgets/KnobWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Knob widget constants.
	*/
	namespace KnobWidgetConstants
	{
		//Define constants.
		constexpr float32 TEXT_FRACTION{ 0.1875f }; //For each text - So times 2 for both the above and below text.
	}

	/*
	*	Default constructor.
	*/
	KnobWidget::KnobWidget() NOEXCEPT
	{
		//Set up the clickable interface.
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
	}

	/*
	*	Callback for when the parent is available.
	*/
	void KnobWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the style from the scene.
		_Style = _Parent->_Parent->GetStyle();
	}

	/*
	*	Renders this widget.
	*/
	void KnobWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Check if this knob is being dragged.
		if (_ClickableInterface.GetState() == UI::ClickableInterface::State::PRESSED)
		{
			//Define constants.
			constexpr float32 REGULAR_SPEED{ 1.0f };
			constexpr float32 SLOW_SPEED{ REGULAR_SPEED * 0.25f };

			//Convert the delta to normalized coordinates.
			const float32 delta{ _ClickableInterface.GetMouseDelta()._Y / _Parent->_Parent->GetReferenceResolution()._Y };

			if (delta != 0.0f)
			{
				//Try to set a new value.
				if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
				{
					_AudioPlugin->SetParameterNormalized(_AudioPluginParameterIndex, _AudioPlugin->GetParameterCurrentNormalized(_AudioPluginParameterIndex) + delta * REGULAR_SPEED);
				}
			}
		}

		//Calculate the center.
		const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(_AxisAlignedBoundingBox) };

		//Calculate the height.
		const float32 height{ _AxisAlignedBoundingBox.LongestSide() };

		//Calculate the text height.
		const float32 text_height{ height * KnobWidgetConstants::TEXT_FRACTION };

		//Calculate the inner/outer circle radius.
		const float32 outer_circle_radius{ height * 0.5f * (1.0f - (KnobWidgetConstants::TEXT_FRACTION * 2.0f)) };
		const float32 inner_circle_radius{ outer_circle_radius * 0.95f };

		//Calculate the indicator thickness.
		const float32 indicator_thickness{ outer_circle_radius - inner_circle_radius };

		//Render the inner circle.
		{
			//Calculate the color.
			const Vector4<float32> color{ GetInnerColor(_ClickableInterface.GetState()) };

			//Render!
			RenderSolidCircle(context, center, outer_circle_radius, color);
		}

		//Draw the indicator.
		{
			//Figure out the angle factor.
			float32 angle_factor{ 0.5f };

			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				angle_factor = _AudioPlugin->GetParameterCurrentNormalized(_AudioPluginParameterIndex);
			}

			//Calculate the up direction.
			Vector2<float32> up_direction{ 0.0f, 1.0f };
			up_direction.Rotate(-BaseMath::LinearlyInterpolate(BaseMath::DegreesToRadians(-135.0f), BaseMath::DegreesToRadians(135.0f), angle_factor));

			//Calculate the start/end.
			const Vector2<float32> start{ center + up_direction * outer_circle_radius * 0.1f };
			const Vector2<float32> end{ center + up_direction * outer_circle_radius };

			//Calculate the color.
			Vector4<float32> color{ GetOuterColor(_ClickableInterface.GetState()) };

			//Always draw at full opacity for the outer circle.
			color._A = 1.0f;

			RenderSolidLine
			(
				context,
				start,
				end,
				indicator_thickness,
				color
			);
		}

		//Render the outer circle.
		{
			//Calculate the color.
			Vector4<float32> color{ GetOuterColor(_ClickableInterface.GetState()) };

			//Always draw at full opacity for the outer circle.
			color._A = 1.0f;

			//Render!
			RenderHollowCircle(context, center, outer_circle_radius, inner_circle_radius, color);
		}

		//Render the text above.
		{
			//Figure out the text.
			const char *RESTRICT text{ nullptr };

			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				text = _AudioPlugin->GetParameterName(_AudioPluginParameterIndex);
			}

			if (text)
			{
				AxisAlignedBoundingBox2D bounding_box{ _AxisAlignedBoundingBox };
				bounding_box._Minimum._Y = bounding_box._Maximum._Y - height * KnobWidgetConstants::TEXT_FRACTION;

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
				bounding_box._Maximum._Y = bounding_box._Minimum._Y + height * KnobWidgetConstants::TEXT_FRACTION;

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
	*	Returns the inner color for the given state.
	*/
	NO_DISCARD Vector4<float32> KnobWidget::GetInnerColor(const UI::ClickableInterface::State state) NOEXCEPT
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
	*	Returns the outer color for the given state.
	*/
	NO_DISCARD Vector4<float32> KnobWidget::GetOuterColor(const UI::ClickableInterface::State state) NOEXCEPT
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