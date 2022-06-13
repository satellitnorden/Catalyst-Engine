#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//Resources.
#include <Resources/Core/FontResource.h>
#include <Resources/Core/ResourcePointer.h>

namespace UserInterfaceUtilities
{

	/*
	*	Calculates an aligned bounding box for text.
	*/
	FORCE_INLINE static void CalculateAlignedBoundingBox(	const Vector2<float32>& original_minimum,
															const Vector2<float32>& original_maximum,
															ResourcePointer<FontResource> font_resource,
															const DynamicString &text,
															const float32 scale,
															const TextHorizontalAlignment horizontal_alignment,
															const TextVerticalAlignment vertical_alignment,
															Vector2<float32> *const RESTRICT new_minimum,
															Vector2<float32> *const RESTRICT new_maximum) NOEXCEPT
	{
		//Calculate the horizontal/vertical extent.
		const float32 horizontal_extent{ original_maximum._X - original_minimum._X };
		const float32 vertical_extent{ original_maximum._Y - original_minimum._Y };

		//Calculate the bounding box for the text.
		Vector2<float32> text_minimum{ FLOAT_MAXIMUM, FLOAT_MAXIMUM };
		Vector2<float32> text_maximum{ -FLOAT_MAXIMUM, -FLOAT_MAXIMUM };

		float32 current_offset_X{ 0.0f };

		for (uint64 i{ 0 }, length{ text.Length() }; i < length; ++i)
		{
			//Cache the chartacter.
			const char character{ text[i] };

			//Calculate the character minimum/maximum.
			Vector2<float32> character_minimum;
			Vector2<float32> character_maximum;

			character_minimum._X = original_minimum._X + current_offset_X + font_resource->_CharacterDescriptions[character]._Bearing._X * scale;
			character_minimum._Y = original_minimum._Y - (font_resource->_CharacterDescriptions[character]._Size._Y + font_resource->_CharacterDescriptions[character]._Bearing._Y) * scale;

			character_maximum._X = character_minimum._X + font_resource->_CharacterDescriptions[character]._Size._X * scale;
			character_maximum._Y = character_minimum._Y + font_resource->_CharacterDescriptions[character]._Size._Y * scale;
			
			//Expand the text minimum/maximum.
			text_minimum = Vector2<float32>::Minimum(text_minimum, character_minimum);
			text_maximum = Vector2<float32>::Maximum(text_maximum, character_maximum);

			//Advance the current offset.
			current_offset_X += font_resource->_CharacterDescriptions[character]._Advance * scale;
		}

		//Calculate the text horizontal/vertical extent.
		const float32 text_horizontal_extent{ text_maximum._X - text_minimum._X };
		const float32 text_vertical_extent{ text_maximum._Y - text_minimum._Y };

		//Calculate the new minimum/maximum.
		*new_minimum = original_minimum;
		*new_maximum = original_maximum;

		switch (horizontal_alignment)
		{
			case TextHorizontalAlignment::LEFT:
			{
				//Nothin to do here.

				break;
			}

			case TextHorizontalAlignment::CENTER:
			{
				new_minimum->_X = new_minimum->_X + ((horizontal_extent - text_horizontal_extent) * 0.5f);

				break;
			}

			case TextHorizontalAlignment::RIGHT:
			{
				new_minimum->_X = new_minimum->_X + (horizontal_extent - text_horizontal_extent);

				break;
			}
		}

		switch (vertical_alignment)
		{
			case TextVerticalAlignment::TOP:
			{
				//Nothin to do here.

				break;
			}

			case TextVerticalAlignment::CENTER:
			{
				new_maximum->_Y = new_maximum->_Y - ((vertical_extent - text_vertical_extent) * 0.5f);

				break;
			}

			case TextVerticalAlignment::BOTTOM:
			{
				new_maximum->_Y = new_maximum->_Y - (vertical_extent - text_vertical_extent);

				break;
			}
		}
	}

	/*
	*	Calculates the normalized coordinate relative to the given resolution.
	*/
	FORCE_INLINE static NO_DISCARD Vector2<float32> CalculateNormalizedCoordinate(const Vector2<uint32> coordinate, const Resolution resolution) NOEXCEPT
	{
		return Vector2<float32>(static_cast<float32>(coordinate._X) / static_cast<float32>(resolution._Width), static_cast<float32>(coordinate._Y) / static_cast<float32>(resolution._Height));
	}

	/*
	*	Calculates the optimal text smoothing factor based on the text scale.
	*/
	FORCE_INLINE static NO_DISCARD float32 CalculateOptimalTextSmoothingFactor(ResourcePointer<FontResource> font_resource, const float32 scale) NOEXCEPT
	{
		float32 biased_scale{ scale };

		for (uint8 i{ 0 }; i < 5; ++i)
		{
			biased_scale = CatalystBaseMath::InverseSquare(biased_scale);
		}

		return CatalystBaseMath::LinearlyInterpolate(0.0'25f, 0.5f, biased_scale);
	}

}