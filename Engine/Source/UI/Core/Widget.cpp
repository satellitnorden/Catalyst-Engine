//Header file.
#include <UI/Core/Widget.h>

//UI.
#include <UI/Core/Utilities.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace UI
{

	//Static variable definitions.
	Curve<float32, 5> Widget::TEXT_SMOOTHING_FACTOR_CURVE
	{
		[]() -> Curve<float32, 5>
		{
			Curve<float32, 5> curve;

			curve.SetValue(0, 0.5f);
			curve.SetValue(1, 0.04'00f);
			curve.SetValue(2, 0.0'25f);
			curve.SetValue(3, 0.0'125f);
			curve.SetValue(4, 0.0'0625f);

			return curve;
		}()
	};

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void Widget::Position() NOEXCEPT
	{
		//Calculate the axis aligned bounding box from the container.
		switch (_Parent->_Layout)
		{
			case UI::Container::Layout::LEFT_TO_RIGHT:
			{
				_AxisAlignedBoundingBox._Minimum = Vector2<float32>
				(
					(_Parent->_Cursor + _Parent->_ScrollOffset),
					_Parent->_AxisAlignedBoundingBox._Minimum._Y
				);
				_AxisAlignedBoundingBox._Maximum = Vector2<float32>
				(
					(_Parent->_Cursor + _Parent->_ScrollOffset) + _Parent->_WidgetSize,
					_Parent->_AxisAlignedBoundingBox._Maximum._Y
				);

				break;
			}

			case UI::Container::Layout::TOP_TO_BOTTOM:
			{
				_AxisAlignedBoundingBox._Minimum = Vector2<float32>
				(
					_Parent->_AxisAlignedBoundingBox._Minimum._X,
					_Parent->_AxisAlignedBoundingBox._Maximum._Y - (_Parent->_Cursor + _Parent->_ScrollOffset) - _Parent->_WidgetSize
				);
				_AxisAlignedBoundingBox._Maximum = Vector2<float32>
				(
					_Parent->_AxisAlignedBoundingBox._Maximum._X,
					_Parent->_AxisAlignedBoundingBox._Maximum._Y - (_Parent->_Cursor + _Parent->_ScrollOffset)
				);

				break;
			}

			case UI::Container::Layout::BOTTOM_TO_TOP:
			{
				_AxisAlignedBoundingBox._Minimum = Vector2<float32>
				(
					_Parent->_AxisAlignedBoundingBox._Minimum._X,
					_Parent->_AxisAlignedBoundingBox._Minimum._Y + (_Parent->_Cursor + _Parent->_ScrollOffset)
				);
				_AxisAlignedBoundingBox._Maximum = Vector2<float32>
				(
					_Parent->_AxisAlignedBoundingBox._Maximum._X,
					_Parent->_AxisAlignedBoundingBox._Minimum._Y + (_Parent->_Cursor + _Parent->_ScrollOffset) + _Parent->_WidgetSize
				);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Apply padding.
		_AxisAlignedBoundingBox._Minimum += _Parent->_WidgetPadding;
		_AxisAlignedBoundingBox._Maximum -= _Parent->_WidgetPadding;

		//Advance the container's cursor.
		_Parent->_Cursor += _Parent->_WidgetSize;
	}

	/*
	*	Renders a box with the given parameters.
	*/
	void Widget::RenderBox
	(
		const UI::RenderContext &context,
		const AxisAlignedBoundingBox2D &axis_aligned_bounding_box,
		const ColorOrTexture &color_or_texture,
		const Vector4<float32> &color_opacity,
		const float32 radius
	) NOEXCEPT
	{
		//Add the command.
		context._RenderCommands->Emplace();
		UI::RenderCommand &command{ context._RenderCommands->Back() };

		//Set the positions.
		command._Positions[0] = Vector4<float32>(axis_aligned_bounding_box._Minimum._X, axis_aligned_bounding_box._Minimum._Y, 0.0f, 1.0f);
		command._Positions[1] = Vector4<float32>(axis_aligned_bounding_box._Minimum._X, axis_aligned_bounding_box._Maximum._Y, 0.0f, 1.0f);
		command._Positions[2] = Vector4<float32>(axis_aligned_bounding_box._Maximum._X, axis_aligned_bounding_box._Maximum._Y, 0.0f, 1.0f);
		command._Positions[3] = Vector4<float32>(axis_aligned_bounding_box._Maximum._X, axis_aligned_bounding_box._Minimum._Y, 0.0f, 1.0f);

		//Set the texture coordinates..
		command._TextureCoordinates[0] = Vector2<float32>(0.0f, 0.0f);
		command._TextureCoordinates[1] = Vector2<float32>(0.0f, 1.0f);
		command._TextureCoordinates[2] = Vector2<float32>(1.0f, 1.0f);
		command._TextureCoordinates[3] = Vector2<float32>(1.0f, 0.0f);

		//Flip the texture coordinates.
		for (uint8 i{ 0 }; i < 4; ++i)
		{
			command._TextureCoordinates[i]._Y = 1.0f - command._TextureCoordinates[i]._Y;
		}

		//Set the mode & color or texture.
		if (color_or_texture.IsColor())
		{
			command._Mode = UI::RenderCommand::Mode::COLOR;
			command._ColorOrTexture = *Color(color_or_texture.GetColor()).Data();
		}
		
		else
		{
			command._Mode = UI::RenderCommand::Mode::TEXTURE;
			command._ColorOrTexture = color_or_texture.GetTextureIndex();
		}

		//Set the color/opacity.
		command._ColorOpacity = *Color(color_opacity).Data();

		//Set the parameters.
		command._Parameter1_float32 = radius;
	}

	/*
	*	Renders text with the given parameters.
	*/
	void Widget::RenderText
	(
		const UI::RenderContext &context,
		const AxisAlignedBoundingBox2D &axis_aligned_bounding_box,
		AssetPointer<FontAsset> font,
		const char *const RESTRICT text,
		const uint64 text_length,
		const float32 scale,
		const UI::HorizontalAlignment horizontal_alignment,
		const UI::VerticalAlignment vertical_alignment,
		const Vector4<float32> &color
	) NOEXCEPT
	{
		//Calculate the aligned minimum/maximum.
		Vector2<float32> aligned_minimum;
		Vector2<float32> aligned_maximum;

		UI::Utilities::CalculateTextAlignedBoundingBox
		(
			axis_aligned_bounding_box._Minimum,
			axis_aligned_bounding_box._Maximum,
			font,
			text,
			text_length,
			scale,
			horizontal_alignment,
			vertical_alignment,
			&aligned_minimum,
			&aligned_maximum
		);

		//Calculate the scale.
		float32 _scale{ scale };

		{
			float32 largest_outside{ 0.0f };

			largest_outside = BaseMath::Maximum<float32>(largest_outside, axis_aligned_bounding_box._Minimum._X - aligned_minimum._X);
			largest_outside = BaseMath::Maximum<float32>(largest_outside, axis_aligned_bounding_box._Minimum._Y - aligned_minimum._Y);
			largest_outside = BaseMath::Maximum<float32>(largest_outside, aligned_maximum._X - axis_aligned_bounding_box._Maximum._X);
			largest_outside = BaseMath::Maximum<float32>(largest_outside, aligned_maximum._Y - axis_aligned_bounding_box._Maximum._Y);

			if (largest_outside > 0.0f)
			{
				_scale *= 1.0f / (1.0f + (largest_outside * 2.0f));

				UI::Utilities::CalculateTextAlignedBoundingBox
				(
					axis_aligned_bounding_box._Minimum,
					axis_aligned_bounding_box._Maximum,
					font,
					text,
					text_length,
					_scale,
					horizontal_alignment,
					vertical_alignment,
					&aligned_minimum,
					&aligned_maximum
				);
			}
		}

		//Gather all characters.
		float32 current_offset{ 0.0f };

		for (uint64 i{ 0 }, length{ text_length }; i < length; ++i)
		{
			//Cache the chartacter.
			const char character{ text[i] };

			//Only draw if it's a valid character.
			if (character < 0)
			{
				continue;
			}

			if (character == '\n')
			{
				continue;
			}

			//Add the command.
			context._RenderCommands->Emplace();
			UI::RenderCommand &command{ context._RenderCommands->Back() };

			//Calculate the bounds.
			Vector2<float32> bounds_minimum;
			Vector2<float32> bounds_maximum;

			bounds_minimum._X = aligned_minimum._X + current_offset;
			bounds_minimum._Y = aligned_minimum._Y;
			bounds_maximum._X = aligned_minimum._X + current_offset + font->_CharacterDescriptions[character]._Size._X * _scale;
			bounds_maximum._Y = aligned_minimum._Y + font->_CharacterDescriptions[character]._Size._Y * _scale;

			bounds_minimum._X += font->_CharacterDescriptions[character]._Offset._X * _scale;
			bounds_maximum._X += font->_CharacterDescriptions[character]._Offset._X * _scale;

			bounds_minimum._Y += font->_CharacterDescriptions[character]._Offset._Y * _scale;
			bounds_maximum._Y += font->_CharacterDescriptions[character]._Offset._Y * _scale;

			//Set the positions.
			command._Positions[0] = Vector4<float32>(bounds_minimum._X, bounds_minimum._Y, 0.0f, 1.0f);
			command._Positions[1] = Vector4<float32>(bounds_minimum._X, bounds_maximum._Y, 0.0f, 1.0f);
			command._Positions[2] = Vector4<float32>(bounds_maximum._X, bounds_maximum._Y, 0.0f, 1.0f);
			command._Positions[3] = Vector4<float32>(bounds_maximum._X, bounds_minimum._Y, 0.0f, 1.0f);

			//Calculate the texture bounds.
			Vector2<float32> texture_bounds_minimum;
			Vector2<float32> texture_bounds_maximum;

			texture_bounds_minimum = font->_CharacterDescriptions[character]._TextureBounds._Minimum;
			texture_bounds_maximum = font->_CharacterDescriptions[character]._TextureBounds._Maximum;

			//Set the texture coordinates.
			command._TextureCoordinates[0] = Vector2<float32>(texture_bounds_minimum._X, texture_bounds_minimum._Y);
			command._TextureCoordinates[1] = Vector2<float32>(texture_bounds_minimum._X, texture_bounds_maximum._Y);
			command._TextureCoordinates[2] = Vector2<float32>(texture_bounds_maximum._X, texture_bounds_maximum._Y);
			command._TextureCoordinates[3] = Vector2<float32>(texture_bounds_maximum._X, texture_bounds_minimum._Y);

			//Set the mode.
			command._Mode = UI::RenderCommand::Mode::TEXT;

			//Set the color/texture.
			command._ColorOrTexture = font->_MasterTextureIndex;

			//Set the color/opacity.
			command._ColorOpacity = *Color(color).Data();

			//Set the smoothing factor.
			command._Parameter1_float32 = TEXT_SMOOTHING_FACTOR_CURVE.Sample(_scale / UI::Constants::REFERENCE_RESOLUTION._Y);

			//Update the current offset.
			current_offset += font->_CharacterDescriptions[character]._Advance * _scale;
		}
	}

}