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
		constexpr float32 TEXT_FRACTION{ 0.1875f };
	}

	/*
	*	Default constructor.
	*/
	KnobWidget::KnobWidget() NOEXCEPT
	{
		//Set up the clickable interface.
		_ClickableInterface._IsInside = [](const Widget *const RESTRICT widget, ClickableInterface *const RESTRICT clickable_interface, const Vector2<float32> position)
		{
			//Retrieve the knob bounding box.
			const AxisAlignedBoundingBox2D knob_bounding_box{ static_cast<const KnobWidget *const RESTRICT>(widget)->GetKnobBoundingBox() };

			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(knob_bounding_box) };

			//Calculate the height.
			const float32 height{ widget->GetAxisAlignedBoundingBox().LongestSide() };

			//Calculate the outer circle radius, and it's square.
			const float32 outer_circle_radius{ height * 0.5f };
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
			const float32 delta{ _ClickableInterface.GetMouseDelta()._Y / _Parent->_Parent->GetReferenceResolution()._Y * REGULAR_SPEED };

			//Add to the accumulated change.
			_AccumulatedChange += delta;

			//Try to set a new value.
			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				if (BaseMath::Absolute<float32>(_AccumulatedChange) >= _AudioPlugin->GetParameterStepNormalized(_AudioPluginParameterIndex))
				{
					_AudioPlugin->SetParameterNormalized(_AudioPluginParameterIndex, _AudioPlugin->GetParameterCurrentNormalized(_AudioPluginParameterIndex) + _AccumulatedChange);
					_AccumulatedChange = 0.0f;
				}
			}
		}

		//Retrieve the text bounding box.
		const AxisAlignedBoundingBox2D text_bounding_box{ GetTextBoundingBox() };

		//Retrieve the knob bounding box.
		const AxisAlignedBoundingBox2D knob_bounding_box{ GetKnobBoundingBox() };

		//If this widget has textures, render the textures instead.
		if (!_TextureIndices.Empty())
		{
			//Figure out the angle factor.
			float32 angle_factor{ 0.5f };

			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				angle_factor = _AudioPlugin->GetParameterCurrentNormalized(_AudioPluginParameterIndex);
			}

			//Figure out the texture index.
			const uint32 texture_index{ static_cast<uint32>(static_cast<float32>(_TextureIndices.LastIndex() * angle_factor)) };

			//Render the box.
			RenderSolidBox(context, knob_bounding_box, _TextureIndices[texture_index]);
		}

		else
		{
			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(knob_bounding_box) };

			//Calculate the inner/outer circle radius.
			const float32 outer_circle_radius{ knob_bounding_box.Dimensions()._Y * 0.5f * (1.0f - (KnobWidgetConstants::TEXT_FRACTION * 2.0f)) };
			const float32 inner_circle_radius{ outer_circle_radius * 0.95f };

			//Render the inner circle.
			{
				//Calculate the color.
				const Vector4<float32> color{ GetInnerColor(_ClickableInterface.GetState()) };

				//Render!
				RenderSolidCircle(context, center, outer_circle_radius, color);
			}

			//Draw the indicator.
			{
				//Calculate the indicator radius.
				const float32 indicator_radius{ (outer_circle_radius - inner_circle_radius) * 0.5f };

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
				const Vector2<float32> end{ center + up_direction * inner_circle_radius };

				//Calculate the color.
				Vector4<float32> color{ GetOuterColor(_ClickableInterface.GetState()) };

				//Always draw at full opacity for the outer circle.
				color._A = 1.0f;

				RenderLine
				(
					context,
					start,
					end,
					indicator_radius,
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
				RenderText
				(
					context,
					text_bounding_box,
					_Parent->_Parent->GetFont(),
					text,
					_Parent->_Parent->GetStyle()._BigTextScale
				);
			}
		}

		//Render the text below.
		if (_ClickableInterface.GetState() == UI::ClickableInterface::State::HOVERED || _ClickableInterface.GetState() == UI::ClickableInterface::State::PRESSED)
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
				RenderText
				(
					context,
					knob_bounding_box,
					_Parent->_Parent->GetFont(),
					text,
					_Parent->_Parent->GetStyle()._SmallTextScale
				);
			}
		}
	}

	/*
	*	Returns the text bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D KnobWidget::GetTextBoundingBox() const NOEXCEPT
	{
		//Calculate the text bounding box.
		const AxisAlignedBoundingBox2D text_bounding_box
		{
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Minimum._X,
				BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._Y, _AxisAlignedBoundingBox._Maximum._Y, 1.0f - KnobWidgetConstants::TEXT_FRACTION)
			),
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Maximum._X,
				_AxisAlignedBoundingBox._Maximum._Y
			)
		};

		return text_bounding_box;
	}

	/*
	*	Returns the knob bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D KnobWidget::GetKnobBoundingBox() const NOEXCEPT
	{
		AxisAlignedBoundingBox2D knob_bounding_box
		{
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Minimum._X,
				_AxisAlignedBoundingBox._Minimum._Y
			),
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Maximum._X,
				BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._Y, _AxisAlignedBoundingBox._Maximum._Y, 1.0f - KnobWidgetConstants::TEXT_FRACTION)
			)
		};

		{
			const Vector2<float32> dimensions{ knob_bounding_box.Dimensions() };

			if (dimensions._X > dimensions._Y)
			{
				const float32 difference{ dimensions._X - dimensions._Y };
				const float32 half_difference{ difference * 0.5f };

				knob_bounding_box._Minimum._X += half_difference;
				knob_bounding_box._Maximum._X -= half_difference;
			}

			else if (dimensions._Y > dimensions._X)
			{
				const float32 difference{ dimensions._Y - dimensions._X };
				const float32 half_difference{ difference * 0.5f };

				knob_bounding_box._Minimum._Y += half_difference;
				knob_bounding_box._Maximum._Y -= half_difference;
			}
		}

		return knob_bounding_box;
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