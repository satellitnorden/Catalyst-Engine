#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/FontAsset.h>

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/RenderingSystem.h>

//UI.
#include <UI/Core/UI.h>

namespace UI
{
	namespace Utilities
	{
		/*
		*	Calculates an aligned bounding box for text.
		*/
		FORCE_INLINE static void CalculateTextAlignedBoundingBox
		(
			const Vector2<float32> &original_minimum,
			const Vector2<float32> &original_maximum,
			AssetPointer<FontAsset> font,
			const char *const RESTRICT text,
			const uint64 text_length,
			const float32 scale,
			const UI::HorizontalAlignment horizontal_alignment,
			const UI::VerticalAlignment vertical_alignment,
			Vector2<float32> *const RESTRICT new_minimum,
			Vector2<float32> *const RESTRICT new_maximum) NOEXCEPT
		{
			*new_minimum = original_minimum;
			*new_maximum = original_maximum;

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

			for (uint64 i{ 0 }, length{ text_length }; i < length; ++i)
			{
				//Cache the chartacter.
				const char character{ text[i] };

				if (character < 0)
				{
					continue;
				}

				//Advance the maximum on the X axis..
				text_maximum._X += font->_CharacterDescriptions[character]._Advance * scale;
			}

			//Calculate the text horizontal/vertical extent.
			const float32 text_horizontal_extent{ text_maximum._X - text_minimum._X };
			const float32 text_vertical_extent{ text_maximum._Y - text_minimum._Y };

			//Calculate the new minimum/maximum.
			switch (horizontal_alignment)
			{
				case UI::HorizontalAlignment::LEFT:
				{
					new_minimum->_X = original_minimum._X;
					new_maximum->_X = original_minimum._X + text_maximum._X;

					break;
				}

				case UI::HorizontalAlignment::CENTER:
				{
					const float32 factor{ (horizontal_extent - text_horizontal_extent) * 0.5f };
					new_minimum->_X = original_minimum._X + factor;
					new_maximum->_X = original_maximum._X - factor;

					break;
				}

				case UI::HorizontalAlignment::RIGHT:
				{
					new_minimum->_X = original_maximum._X - text_maximum._X;
					new_maximum->_X = original_maximum._X;

					break;
				}
			}

			switch (vertical_alignment)
			{
				case UI::VerticalAlignment::TOP:
				{
					new_minimum->_Y = original_maximum._Y - baseline_height;
					new_maximum->_Y = original_maximum._Y;

					break;
				}

				case UI::VerticalAlignment::CENTER:
				{
					const float32 factor{ (vertical_extent - text_vertical_extent) * 0.5f };
					new_minimum->_Y = original_minimum._Y + factor;
					new_maximum->_Y = original_maximum._Y - factor;

					break;
				}

				case UI::VerticalAlignment::BOTTOM:
				{
					new_minimum->_Y = original_minimum._Y;
					new_maximum->_Y = original_minimum._Y + baseline_height;

					break;
				}
			}
		}
	}
}