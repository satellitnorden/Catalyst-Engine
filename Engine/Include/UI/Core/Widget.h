#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/FontAsset.h>

//Math.
#include <Math/General/Curve.h>

//UI.
#include <UI/Core/ColorOrTexture.h>
#include <UI/Core/Identifier.h>
#include <UI/Core/ClickableInterface.h>
#include <UI/Core/Container.h>
#include <UI/Core/ScrollableInterface.h>
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
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Widget() NOEXCEPT
		{

		}

		/*
		*	Sets the parent.
		*/
		FORCE_INLINE void SetParent(UI::Container *const RESTRICT value) NOEXCEPT
		{
			_Parent = value;
		}

		/*
		*	Callback for when the parent is available.
		*/
		FORCE_INLINE virtual void OnParentAvailable() NOEXCEPT
		{

		}

		/*
		*	Positions this widget. Usually called right after creation.
		*/
		virtual void Position() NOEXCEPT;

		/*
		*	Returns whether or not this widget is enabled.
		*/
		FORCE_INLINE NO_DISCARD bool IsEnabled() const NOEXCEPT
		{
			return _Enabled;
		}

		/*
		*	Sets whether or not this widget is enabled.
		*/
		FORCE_INLINE void SetEnabled(const bool value) NOEXCEPT
		{
			_Enabled = value;
		}

		/*
		*	Returns the axis aligned bounding box.
		*/
		FORCE_INLINE NO_DISCARD const AxisAlignedBoundingBox2D &GetAxisAlignedBoundingBox() const NOEXCEPT
		{
			return _AxisAlignedBoundingBox;
		}

		/*
		*	Sets the axis aligned bounding box.
		*/
		FORCE_INLINE Widget *const RESTRICT SetAxisAlignedBoundingBox(const AxisAlignedBoundingBox2D &value) NOEXCEPT
		{
			_AxisAlignedBoundingBox = value;
			return this;
		}

		/*
		*	Returns the clickable interface (if this widget is clickable.
		*/
		FORCE_INLINE virtual NO_DISCARD UI::ClickableInterface *const RESTRICT GetClickableInterface() NOEXCEPT
		{
			return nullptr;
		}

		/*
		*	Returns the scrollable interface (if this widget is scrollable).
		*/
		FORCE_INLINE virtual NO_DISCARD UI::ScrollableInterface *const RESTRICT GetScrollableInterface() NOEXCEPT
		{
			return nullptr;
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
		*	Renders a box with the given parameters.
		*/
		void RenderBox
		(
			const UI::RenderContext &context,
			const AxisAlignedBoundingBox2D &axis_aligned_bounding_box,
			const ColorOrTexture &color_or_texture,
			const Vector4<float32> &color_opacity = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f),
			const float32 radius = 0.0f
		) NOEXCEPT;

		/*
		*	Renders text with the given parameters.
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
			const UI::VerticalAlignment vertical_alignment = UI::VerticalAlignment::CENTER,
			const Vector4<float32> &color = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f)
		) NOEXCEPT;

	private:

		//Denotes whether or not this widget is enabled.
		bool _Enabled{ true };

	};

}