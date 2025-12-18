#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/FontAsset.h>

//Math.
#include <Math/General/Curve.h>

//UI.
#include <UI/Core/Identifier.h>
#include <UI/Core/Container.h>
#include <UI/Core/RenderContext.h>

namespace UI
{

	/*
	*	Base class for all widgets.
	*/
	class Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		Widget(UI::Container *const RESTRICT parent) NOEXCEPT;

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Widget() NOEXCEPT
		{

		}

		/*
		*	Renders this widget.
		*/
		FORCE_INLINE virtual void Render(const UI::RenderContext &context) NOEXCEPT
		{

		}

	protected:
		
		//The text smoothing factor curve.
		static Curve<float32, 5> TEXT_SMOOTHING_FACTOR_CURVE;

		//The parent.
		UI::Container *RESTRICT _Parent;

		//The axis aligned bounding box.
		AxisAlignedBoundingBox2D _AxisAlignedBoundingBox;

		/*
		*	Renders text in the given axis aligned bounding box.
		*/
		void RenderText
		(
			const UI::RenderContext &context,
			const AxisAlignedBoundingBox2D &axis_aligned_bounding_box,
			AssetPointer<FontAsset> font,
			const char *const RESTRICT text,
			const uint64 text_length,
			const float32 scale,
			const UI::HorizontalAlignment horizontal_alignment = UI::HorizontalAlignment::CENTER,
			const UI::VerticalAlignment vertical_alignment = UI::VerticalAlignment::CENTER
		) NOEXCEPT;

	};

}