//Header file.
#include <UI/Widgets/KnobWidget.h>

//Input.
#include <Input/InputCore.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

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
			const float32 height{ knob_bounding_box.Dimensions()._Y };

			//Calculate the outer circle radius, and it's square.
			const float32 outer_circle_radius{ height * 0.5f };
			const float32 outer_circle_radius_squared{ outer_circle_radius * outer_circle_radius };

			//Calculate the distance to the center.
			const float32 distance_to_center{ Vector2<float32>::LengthSquared(center - position) };

			//The mouse is inside if it is inside the knob circle.
			return distance_to_center <= outer_circle_radius_squared;
		};
		_ClickableInterface.SetOnStartPressCallback
		(
			[](Widget *const RESTRICT widget, void *const RESTRICT /*user_data*/)
			{
				static_cast<KnobWidget *const RESTRICT>(widget)->OnPressed();
			}
		);
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
			constexpr float32 REGULAR_SPEED{ 2.0f };
			constexpr float32 SLOW_SPEED{ REGULAR_SPEED * 0.25f };

			//Convert the delta to normalized coordinates.
			const float32 delta{ _ClickableInterface.GetMouseDelta()._Y / _Parent->_Parent->GetReferenceResolution()._Y * REGULAR_SPEED };

			//Add to the accumulated change.
			_AccumulatedChange += delta;

			//Try to set a new value.
			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				const float32 current{ _AudioPlugin->GetParameterCurrentNormalized(_AudioPluginParameterIndex) };

				if (_AudioPlugin->IsParameterStepped(_AudioPluginParameterIndex))
				{
					const float32 parameter_step{ _AudioPlugin->GetParameterStepNormalized(_AudioPluginParameterIndex) };

					if (BaseMath::Absolute<float32>(_AccumulatedChange) >= parameter_step)
					{
						_AudioPlugin->SetParameterNormalized(_AudioPluginParameterIndex, current + parameter_step * BaseMath::Signum(_AccumulatedChange));
						_AccumulatedChange = 0.0f;
					}
				}
				
				else
				{
					_AudioPlugin->SetParameterNormalized(_AudioPluginParameterIndex, current + _AccumulatedChange);
					_AccumulatedChange = 0.0f;
				}
			}
		}

		//Render the text above.
		{
			//Retrieve the bounding box.
			const AxisAlignedBoundingBox2D above_text_bounding_box{ GetAboveTextBoundingBox() };

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
					above_text_bounding_box,
					_Parent->_Parent->GetStyle()._Font,
					text,
					_Parent->_Parent->GetStyle()._MediumTextScale
				);
			}
		}

		//Render the knob.
		{
			//Retrieve the bounding box.
			const AxisAlignedBoundingBox2D knob_bounding_box{ GetKnobBoundingBox() };

			//Figure out the angle factor.
			float32 angle_factor{ 0.5f };

			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				angle_factor = _AudioPlugin->GetParameterCurrentNormalized(_AudioPluginParameterIndex);
			}

			//Render differently depending on if we have texture indices or not.
			if (_TextureIndices.Empty())
			{
				//Calculate the center.
				const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(knob_bounding_box) };

				//Calculate the inner/outer circle radius.
				const float32 outer_circle_radius{ knob_bounding_box.Dimensions()._Y * 0.5f };
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

			else
			{
				//Figure out the texture index.
				const uint32 texture_index{ static_cast<uint32>(static_cast<float32>(_TextureIndices.LastIndex() * angle_factor)) };

				//Render the box.
				RenderSolidBox(context, knob_bounding_box, _TextureIndices[texture_index]);
			}
		}

		//Render the text below.
		{
			//Figure out if the text below should be drawn, and if so, which bounding box to use.
			bool draw_text_below{ true };
			AxisAlignedBoundingBox2D text_below_bounding_box;

			//If we have texture indices, draw it on top of the knob, but only if it's hovered or pressed.
			if (_TextureIndices.Empty())
			{
				draw_text_below = true;
				text_below_bounding_box = GetBelowTextBoundingBox();
			}

			else
			{
				if (_ClickableInterface.GetState() == UI::ClickableInterface::State::HOVERED || _ClickableInterface.GetState() == UI::ClickableInterface::State::PRESSED)
				{
					draw_text_below = true;
					text_below_bounding_box = GetKnobBoundingBox();
				}

				else
				{
					draw_text_below = false;
				}
			}

			if (draw_text_below)
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
						text_below_bounding_box,
						_Parent->_Parent->GetStyle()._Font,
						text,
						_Parent->_Parent->GetStyle()._MediumTextScale
					);
				}
			}
		}

		//Update the time since the last press.
		_TimeSinceLastPress += context._DeltaTime;
	}

	/*
	*	Callback for when this knob is pressed.
	*/
	void KnobWidget::OnPressed() NOEXCEPT
	{
		//If the time since the last press is lower than the double click threshold, reset the parameter.
		if (_TimeSinceLastPress < InputConstants::DOUBLE_CLICK_THRESHOLD)
		{
			if (_AudioPlugin != nullptr && _AudioPluginParameterIndex != UINT64_MAXIMUM)
			{
				_AudioPlugin->SetParameterPlain(_AudioPluginParameterIndex, _AudioPlugin->GetParameterDefaultPlain(_AudioPluginParameterIndex));
			}

			//Also reset the accumulated change.
			_AccumulatedChange = 0.0f;
		}

		//Reset the time since the last press.
		_TimeSinceLastPress = 0.0f;
	}

	/*
	*	Returns the above text bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D KnobWidget::GetAboveTextBoundingBox() const NOEXCEPT
	{
		//Calculate the text bounding box.
		const AxisAlignedBoundingBox2D text_bounding_box
		{
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Minimum._X,
				_AxisAlignedBoundingBox._Maximum._Y - _Parent->_Parent->GetStyle()._MediumTextScale
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
	*	Returns the below text bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D KnobWidget::GetBelowTextBoundingBox() const NOEXCEPT
	{
		//Calculate the text bounding box.
		const AxisAlignedBoundingBox2D text_bounding_box
		{
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Minimum._X,
				_AxisAlignedBoundingBox._Minimum._Y
			),
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Maximum._X,
				_AxisAlignedBoundingBox._Minimum._Y + _Parent->_Parent->GetStyle()._MediumTextScale
			)
		};

		return text_bounding_box;
	}

	/*
	*	Returns the knob bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D KnobWidget::GetKnobBoundingBox() const NOEXCEPT
	{
		//Define constants.
		constexpr float32 PADDING{ 4.0f };

		//Handle differently depending on if we have texture indices or not.
		AxisAlignedBoundingBox2D knob_bounding_box;

		if (_TextureIndices.Empty())
		{
			knob_bounding_box = AxisAlignedBoundingBox2D
			{
				Vector2<float32>
				(
					_AxisAlignedBoundingBox._Minimum._X,
					_AxisAlignedBoundingBox._Minimum._Y + _Parent->_Parent->GetStyle()._MediumTextScale
				),
				Vector2<float32>
				(
					_AxisAlignedBoundingBox._Maximum._X,
					_AxisAlignedBoundingBox._Maximum._Y - _Parent->_Parent->GetStyle()._MediumTextScale
				)
			};

			knob_bounding_box._Minimum._Y += PADDING;
			knob_bounding_box._Maximum._Y -= PADDING;
		}

		else
		{
			knob_bounding_box = AxisAlignedBoundingBox2D
			{
				Vector2<float32>
				(
					_AxisAlignedBoundingBox._Minimum._X,
					_AxisAlignedBoundingBox._Minimum._Y
				),
				Vector2<float32>
				(
					_AxisAlignedBoundingBox._Maximum._X,
					_AxisAlignedBoundingBox._Maximum._Y - _Parent->_Parent->GetStyle()._MediumTextScale
				)
			};
		}

		//Always make it square.
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