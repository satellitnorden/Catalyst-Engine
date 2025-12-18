#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Container.h>
#include <UI/Core/BuildContext.h>
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Base class for all UI scenes.
	*/
	class Scene
	{

	public:

		/*
		*	Default constructor.
		*/
		Scene() NOEXCEPT;

		/*
		*	Default destructor.
		*/
		FORCE_INLINE ~Scene() NOEXCEPT
		{

		}

		/*
		*	Returns the font for this scene.
		*/
		FORCE_INLINE NO_DISCARD const AssetPointer<FontAsset> GetFont() const NOEXCEPT
		{
			return _Font;
		}

		/*
		*	Returns the text scale for this scene.
		*/
		FORCE_INLINE NO_DISCARD float32 GetTextScale() const NOEXCEPT
		{
			return _TextScale;
		}

		/*
		*	Prepares this scene for building.
		*/
		FORCE_INLINE void PrepareBuild() NOEXCEPT
		{
			//Clear the widgets.
			_Widgets.Clear();
		}

		/*
		*	Builds this scene.
		*/
		FORCE_INLINE virtual void Build(const UI::BuildContext &context) NOEXCEPT = 0;
		
		/*
		*	Renders this scene.
		*/
		FORCE_INLINE void Render(const UI::RenderContext &context) NOEXCEPT
		{
			for (UI::Widget *const RESTRICT widget : _Widgets)
			{
				widget->Render(context);
			}
		}

	protected:

		/*
		*	Sets the font for this scene.
		*/
		FORCE_INLINE void SetFont(const AssetPointer<FontAsset> value) NOEXCEPT
		{
			_Font = value;
		}

		/*
		*	Sets the text scale for this scene.
		*/
		FORCE_INLINE void SetTextScale(const float32 value) NOEXCEPT
		{
			_TextScale = value;
		}

		/*
		*	Starts a container with the given identifier.
		*/
		void StartContainer(const UI::Identifier identifier, const UI::Anchor anchor, const UI::Container::Layout layout, const AxisAlignedBoundingBox2D &axis_aligned_bounding_box) NOEXCEPT;

		/*
		*	Sets the container layout.
		*	Note that this will reset the cursor.
		*/
		void SetContainerLayout(const UI::Container::Layout layout) NOEXCEPT;

		/*
		*	Sets the container widget size.
		*	This is just a single value, because widgets by default take up all of one dimension, depending on layout;
		*	If the layout is horizontal, widgets will have [widget_size, container_size._Y] size,
		*	and if the layout is vertical, widgets will have [container_size._X, widget_size] size.
		*/
		void SetContainerWidgetSize(const float32 size) NOEXCEPT;

		/*
		*	Sets the container widget padding.
		*/
		void SetContainerWidgetPadding(const float32 padding) NOEXCEPT;

		/*
		*	Adds a widget of the given class.
		*/
		template <typename TYPE>
		FORCE_INLINE TYPE *const RESTRICT AddWidget(const UI::BuildContext &context, const UI::Identifier identifier) NOEXCEPT
		{
			//Sanity check.
			ASSERT(_Container._Active, "Can't add widgets without an active container!");

			//Calculate the combined identifier.
			const UI::Identifier combined_identifier{ _Container._Identifier, identifier };

			//Allocate the widget!
			TYPE *const RESTRICT widget{ context._WidgetAllocator->Allocate<TYPE>(combined_identifier, &_Container) };

			//Add the widget to the widgets array.
			_Widgets.Emplace(widget);

			//Return the widget.
			return widget;
		}

		/*
		*	Ends the active container.
		*/
		void EndContainer() NOEXCEPT;

	private:

		//The widgets.
		DynamicArray<UI::Widget *RESTRICT> _Widgets;

		//The container.
		UI::Container _Container;

		//The font.
		AssetPointer<FontAsset> _Font;

		//The text scale.
		float32 _TextScale;

	};

}