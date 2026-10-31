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
		constexpr float32 TEXT_FRACTION{ 0.1875f };
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

		//Retrieve the text bounding box.
		const AxisAlignedBoundingBox2D text_bounding_box{ GetTextBoundingBox() };

		//Retrieve the switch bounding box.
		const AxisAlignedBoundingBox2D switch_bounding_box{ GetSwitchBoundingBox() };

		//Render the indicator.
		if (_HasTextureIndices)
		{
			//Cache the switch state.
			const bool switch_state{ _AudioPlugin->GetParameterCurrentPlain(_AudioPluginParameterIndex) >= 0.5f };

			//Render the box.
			RenderSolidBox(context, switch_bounding_box, _TextureIndices[static_cast<uint32>(switch_state)]);
		}

		else
		{
			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(switch_bounding_box) };

			//Calculate the indicator height.
			const float32 indicator_height{ switch_bounding_box.Dimensions()._Y };

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
				RenderText
				(
					context,
					text_bounding_box,
					_Parent->_Parent->GetFont(),
					text,
					_Style._MediumTextScale
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
	*	Returns the text bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D SwitchWidget::GetTextBoundingBox() const NOEXCEPT
	{
		//Calculate the text bounding box.
		const AxisAlignedBoundingBox2D text_bounding_box
		{
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Minimum._X,
				BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._Y, _AxisAlignedBoundingBox._Maximum._Y, 1.0f - SwitchWidgetConstants::TEXT_FRACTION)
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
	*	Returns the switch bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D SwitchWidget::GetSwitchBoundingBox() const NOEXCEPT
	{
		AxisAlignedBoundingBox2D switch_bounding_box
		{
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Minimum._X,
				_AxisAlignedBoundingBox._Minimum._Y
			),
			Vector2<float32>
			(
				_AxisAlignedBoundingBox._Maximum._X,
				BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._Y, _AxisAlignedBoundingBox._Maximum._Y, 1.0f - SwitchWidgetConstants::TEXT_FRACTION)
			)
		};

		{
			const Vector2<float32> dimensions{ switch_bounding_box.Dimensions() };

			if (dimensions._X > dimensions._Y)
			{
				const float32 difference{ dimensions._X - dimensions._Y };
				const float32 half_difference{ difference * 0.5f };

				switch_bounding_box._Minimum._X += half_difference;
				switch_bounding_box._Maximum._X -= half_difference;
			}

			else if (dimensions._Y > dimensions._X)
			{
				const float32 difference{ dimensions._Y - dimensions._X };
				const float32 half_difference{ difference * 0.5f };

				switch_bounding_box._Minimum._Y += half_difference;
				switch_bounding_box._Maximum._Y -= half_difference;
			}
		}

		return switch_bounding_box;
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