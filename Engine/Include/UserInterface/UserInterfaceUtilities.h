#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//Systems.
#include <Systems/RenderingSystem.h>

namespace UserInterfaceUtilities
{

	/*
	*	Calculates an aligned bounding box for text.
	*/
	FORCE_INLINE static void CalculateAlignedBoundingBox(	const Vector2<float32> &original_minimum,
															const Vector2<float32> &original_maximum,
															AssetPointer<FontAsset> font,
															const DynamicString &text,
															const float32 scale,
															const TextHorizontalAlignment horizontal_alignment,
															const TextVerticalAlignment vertical_alignment,
															Vector2<float32> *const RESTRICT new_minimum,
															Vector2<float32> *const RESTRICT new_maximum) NOEXCEPT
	{
		*new_minimum = original_minimum;
		*new_maximum = original_maximum;
#if 1
		//Calculate the aspect ratio reciprocal.
		const float32 aspect_ratio_reciprocal{ 1.0f / RenderingSystem::Instance->GetFullAspectRatio() };

		//Use the character 'A' as a baseline height.
		const float32 baseline_height{ font->_CharacterDescriptions['A']._Size._Y * scale };

		//Calculate the original center.
		const Vector2<float32> original_center{ BaseMath::LinearlyInterpolate(original_minimum, original_maximum, 0.5f) };

		//Calculate the horizontal/vertical extent.
		const float32 horizontal_extent{ original_maximum._X - original_minimum._X };
		const float32 vertical_extent{ original_maximum._Y - original_minimum._Y };

		//Calculate the bounding box for the text.
		Vector2<float32> text_minimum{ 0.0f, original_center._Y - (baseline_height * 0.5f) };
		Vector2<float32> text_maximum{ 0.0f, original_center._Y + (baseline_height * 0.5f) };

		for (uint64 i{ 0 }, length{ text.Length() }; i < length; ++i)
		{
			//Cache the chartacter.
			const char character{ text[i] };

			if (character < 0)
			{
				continue;
			}

			//Advance the maximum on the X axis..
			text_maximum._X += font->_CharacterDescriptions[character]._Advance * scale * aspect_ratio_reciprocal;
		}

		//Calculate the text horizontal/vertical extent.
		const float32 text_horizontal_extent{ text_maximum._X - text_minimum._X };
		const float32 text_vertical_extent{ text_maximum._Y - text_minimum._Y };

		//Calculate the new minimum/maximum.
		switch (horizontal_alignment)
		{
			case TextHorizontalAlignment::LEFT:
			{
				new_minimum->_X = original_minimum._X;
				new_maximum->_X = original_minimum._X + text_maximum._X;

				break;
			}

			case TextHorizontalAlignment::CENTER:
			{
				const float32 factor{ (horizontal_extent - text_horizontal_extent) * 0.5f };
				new_minimum->_X = original_minimum._X + factor;
				new_maximum->_X = original_maximum._X - factor;

				break;
			}

			case TextHorizontalAlignment::RIGHT:
			{
				new_minimum->_X = original_maximum._X - text_maximum._X;
				new_maximum->_X = original_maximum._X;

				break;
			}
		}

		switch (vertical_alignment)
		{
			case TextVerticalAlignment::TOP:
			{
				new_minimum->_Y = original_maximum._Y - baseline_height;
				new_maximum->_Y = original_maximum._Y;

				break;
			}

			case TextVerticalAlignment::CENTER:
			{
				const float32 factor{ (vertical_extent - text_vertical_extent) * 0.5f };
				new_minimum->_Y = original_minimum._Y + factor;
				new_maximum->_Y = original_maximum._Y - factor;

				break;
			}

			case TextVerticalAlignment::BOTTOM:
			{
				new_minimum->_Y = original_minimum._Y;
				new_maximum->_Y = original_minimum._Y + baseline_height;

				break;
			}
		}
#else
		//Use the character 'A' as a baseline height.
		const float32 baseline_height{ font_resource->_CharacterDescriptions['A']._Size._Y };

		//Calculate the aspect ratio reciprocal.
		const float32 aspect_ratio_reciprocal{ 1.0f / RenderingSystem::Instance->GetFullAspectRatio() };

		//Calculate the horizontal/vertical extent.
		const float32 horizontal_extent{ original_maximum._X - original_minimum._X };
		const float32 vertical_extent{ original_maximum._Y - original_minimum._Y };

		//Calculate the bounding box for the text.
		Vector2<float32> text_minimum{ FLOAT32_MAXIMUM, FLOAT32_MAXIMUM };
		Vector2<float32> text_maximum{ -FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM };

		float32 current_offset_X{ 0.0f };

		for (uint64 i{ 0 }, length{ text.Length() }; i < length; ++i)
		{
			//Cache the chartacter.
			const char character{ text[i] };

			//Calculate the character minimum/maximum.
			Vector2<float32> character_minimum;
			Vector2<float32> character_maximum;

			character_minimum._X = original_minimum._X + current_offset_X;
			character_minimum._Y = original_minimum._Y - (font_resource->_CharacterDescriptions[character]._Size._Y + font_resource->_CharacterDescriptions[character]._Offset) * scale;

			character_maximum._X = character_minimum._X + font_resource->_CharacterDescriptions[character]._Size._X * scale;
			character_maximum._Y = character_minimum._Y + font_resource->_CharacterDescriptions[character]._Size._Y * scale;
			
			//Expand the text minimum/maximum.
			text_minimum = Vector2<float32>::Minimum(text_minimum, character_minimum);
			text_maximum = Vector2<float32>::Maximum(text_maximum, character_maximum);

			//Advance the current offset.
			current_offset_X += font_resource->_CharacterDescriptions[character]._Advance * scale;
		}

		//Augment the maximum.
		text_maximum._Y = text_minimum._Y + (font_resource->_DefaultHeight * RenderingSystem::Instance->GetFullAspectRatio() * scale);

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
				//Nothing to do here.

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
				//Nothing to do here.

				break;
			}

			case TextVerticalAlignment::CENTER:
			{
				new_minimum->_Y = original_minimum._Y + ((vertical_extent - text_vertical_extent) * 0.5f);
				new_maximum->_Y = new_minimum->_Y + text_vertical_extent;

				break;
			}

			case TextVerticalAlignment::BOTTOM:
			{
				new_maximum->_Y = new_maximum->_Y - (vertical_extent - text_vertical_extent);

				break;
			}
		}
#endif
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
	FORCE_INLINE static NO_DISCARD float32 CalculateOptimalTextSmoothingFactor(AssetPointer<FontAsset> font, const float32 scale) NOEXCEPT
	{
		return 1.0f / 8.0f;
	}

}