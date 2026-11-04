//Header file.
#include <UI/Widgets/SwitchWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	SwitchWidget::SwitchWidget() NOEXCEPT
	{
		//Set up the clickable interface.
		//Set up the clickable interface.
		_ClickableInterface._IsInside = [](const Widget *const RESTRICT widget, ClickableInterface *const RESTRICT clickable_interface, const Vector2<float32> position)
		{
			//Cache the switch widget.
			const SwitchWidget *const RESTRICT switch_widget{ static_cast<const SwitchWidget *const RESTRICT>(widget) };

			//Retrieve the switch bounding box.
			AxisAlignedBoundingBox2D switch_bounding_box{ switch_widget->GetSwitchBoundingBox() };

			//If we have texture indices, just return if it is inside.
			if (switch_widget->_HasTextureIndices)
			{
				return switch_bounding_box.IsInside(position);
			}

			//Calculate the center.
			const Vector2<float32> center{ AxisAlignedBoundingBox2D::CalculateCenter(switch_bounding_box) };

			//Calculate the height.
			const float32 height{ switch_bounding_box.Dimensions()._Y };

			//Calculate the width.
			const float32 width{ height * 0.5f };

			//Shrink the switch bounding box.
			switch_bounding_box._Minimum._X = center._X - width * 0.5f;
			switch_bounding_box._Maximum._X = center._X + width * 0.5f;

			//Return if the position is inside.
			return switch_bounding_box.IsInside(position);
		};
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
					_Style._MediumTextScale
				);
			}
		}

		//Render the switch.
		{
			//Retrieve the bounding box.
			const AxisAlignedBoundingBox2D switch_bounding_box{ GetSwitchBoundingBox() };

			//Cache the switch state.
			const bool switch_state{ _AudioPlugin->GetParameterCurrentPlain(_AudioPluginParameterIndex) >= 0.5f };

			//Render differently depending on if we have texture indices or not.
			if (!_HasTextureIndices)
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

			else
			{
				//Render the box.
				RenderSolidBox(context, switch_bounding_box, _TextureIndices[static_cast<uint32>(switch_state)]);
			}
		}
		
		//Render the text below.
		{
			//Figure out if the text below should be drawn, and if so, which bounding box to use.
			bool draw_text_below{ true };
			AxisAlignedBoundingBox2D text_below_bounding_box;

			//If we have texture indices, draw it on top of the swwitch, but only if it's hovered or pressed.
			if (!_HasTextureIndices)
			{
				draw_text_below = true;
				text_below_bounding_box = GetBelowTextBoundingBox();
			}

			else
			{
				if (_ClickableInterface.GetState() == UI::ClickableInterface::State::HOVERED || _ClickableInterface.GetState() == UI::ClickableInterface::State::PRESSED)
				{
					draw_text_below = true;
					text_below_bounding_box = GetSwitchBoundingBox();
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
	*	Returns the above text bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D SwitchWidget::GetAboveTextBoundingBox() const NOEXCEPT
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
	NO_DISCARD AxisAlignedBoundingBox2D SwitchWidget::GetBelowTextBoundingBox() const NOEXCEPT
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
	*	Returns the switch bounding box.
	*/
	NO_DISCARD AxisAlignedBoundingBox2D SwitchWidget::GetSwitchBoundingBox() const NOEXCEPT
	{
		//Define constants.
		constexpr float32 PADDING{ 4.0f };

		//Handle differently depending on if we have texture indices or not.
		AxisAlignedBoundingBox2D switch_bounding_box;

		if (!_HasTextureIndices)
		{
			switch_bounding_box = AxisAlignedBoundingBox2D
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

			switch_bounding_box._Minimum._Y += PADDING;
			switch_bounding_box._Maximum._Y -= PADDING;
		}

		else
		{
			switch_bounding_box = AxisAlignedBoundingBox2D
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

		//Make it square if we have texture indices.
		if (_HasTextureIndices)
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