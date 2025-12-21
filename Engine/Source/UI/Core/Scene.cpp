//Header file.
#include <UI/Core/Scene.h>

//Systems.
#include <Systems/ContentSystem.h>

//UI.
#include <UI/Widgets/ScrollableWidget.h>
#include <UI/Widgets/ScrollBarWidget.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	Scene::Scene() NOEXCEPT
	{
		//Set up the identifier.
		{
			static Atomic<uint64> COUNTER{ 0 };
			_Identifier = COUNTER.FetchAdd(1);
		}

		//Set the default font.
		_Font = ContentSystem::Instance->GetAsset<FontAsset>(HashString("Default"));

		//Set the default text scale.
		_TextScale = 32.0f;
	}

	/*
	*	Prepares this scene for building.
	*/
	void Scene::PrepareBuild() NOEXCEPT
	{
		//Remove all containers that are no longer alive, and reset them.
		for (uint64 container_index{ 0 }; container_index < _Containers.Size();)
		{
			UI::Container &container{ _Containers[container_index] };

			if (container._Alive)
			{
				container._Alive = false;

				++container_index;
			}

			else
			{
				_Containers.EraseAt<false>(container_index);
			}
		}

		//Clear the widgets.
		_Widgets.Clear();
	}

	/*
	*	Starts a container with the given identifier.
	*/
	void Scene::StartContainer
	(
		const UI::BuildContext &context,
		const UI::Identifier identifier,
		const UI::Anchor anchor,
		const UI::Container::Layout layout,
		const AxisAlignedBoundingBox2D &axis_aligned_bounding_box
	) NOEXCEPT
	{
		ASSERT(!_ActiveContainer, "Trying to start a container while another is active!");

		//Determine if we need to patch up pointers.
		bool need_to_patch_up_pointers{ false };

		//Find or create the container.
		{
			bool found_container{ false };

			for (UI::Container &container : _Containers)
			{
				if (container._Identifier == identifier)
				{
					_ActiveContainer = &container;
					found_container = true;
					break;
				}
			}

			if (!found_container)
			{
				need_to_patch_up_pointers = _Containers.Size() == _Containers.Capacity();
				_Containers.Emplace();
				_ActiveContainer = &_Containers.Back();

				_ActiveContainer->_ScrollOffset = 0.0f;
			}
		}

		//This container is now alive.
		_ActiveContainer->_Alive = true;

		//Patch up pointers for old containers, if necessary.
		if (need_to_patch_up_pointers)
		{
			for (UI::Container &old_container : _Containers)
			{
				for (uint64 widget_index{ old_container._StartWidgetIndex }; widget_index < old_container._EndWidgetIndex; ++widget_index)
				{
					_Widgets[widget_index]->SetParent(&old_container);
				}
			}
		}

		//Set the parent.
		_ActiveContainer->_Parent = this;

		//Copy settings.
		_ActiveContainer->_Identifier = identifier;
		_ActiveContainer->_Anchor = anchor;
		_ActiveContainer->_Layout = layout;

		//Set up the axis aligned bounding box.
		{
			//Calculate commonly used values.
			const Vector2<float32> center{ BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum, axis_aligned_bounding_box._Maximum, 0.5f) };
			const Vector2<float32> size{ axis_aligned_bounding_box._Maximum - axis_aligned_bounding_box._Minimum };
			const Vector2<float32> half_size{ size * 0.5f };

			//Set up the bounding box differently depending on the anchor.
			switch (anchor)
			{
				case UI::Anchor::CENTER:
				{
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._X = UI::Constants::REFERENCE_RESOLUTION._X * 0.5f - half_size._X;
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._Y = UI::Constants::REFERENCE_RESOLUTION._Y * 0.5f - half_size._Y;

					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._X = UI::Constants::REFERENCE_RESOLUTION._X * 0.5f + half_size._X;
					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._Y = UI::Constants::REFERENCE_RESOLUTION._Y * 0.5f + half_size._Y;

					break;
				}

				case UI::Anchor::TOP:
				{
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._X = center._X - half_size._X;
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._Y = UI::Constants::REFERENCE_RESOLUTION._Y - size._Y;

					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._X = center._X + half_size._X;
					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._Y = UI::Constants::REFERENCE_RESOLUTION._Y;

					break;
				}

				case UI::Anchor::LEFT:
				{
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._X = 0.0f;
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._Y = UI::Constants::REFERENCE_RESOLUTION._Y * 0.5f - half_size._Y;

					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._X = size._X;
					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._Y = UI::Constants::REFERENCE_RESOLUTION._Y * 0.5f + half_size._Y;

					break;
				}

				case UI::Anchor::BOTTOM_LEFT:
				{
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._X = 0.0f;
					_ActiveContainer->_AxisAlignedBoundingBox._Minimum._Y = 0.0f;

					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._X = size._X;
					_ActiveContainer->_AxisAlignedBoundingBox._Maximum._Y = size._Y;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}
		}

		//Set up the (default) widget size.
		switch (layout)
		{
			case UI::Container::Layout::LEFT_TO_RIGHT:
			case UI::Container::Layout::RIGHT_TO_LEFT:
			{
				_ActiveContainer->_WidgetSize = UI::Constants::REFERENCE_RESOLUTION._X * 0.1f;

				break;
			}

			case UI::Container::Layout::TOP_TO_BOTTOM:
			case UI::Container::Layout::BOTTOM_TO_TOP:
			{
				_ActiveContainer->_WidgetSize = UI::Constants::REFERENCE_RESOLUTION._Y * 0.1f;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set up the (default) widget padding.
		_ActiveContainer->_WidgetPadding = 8.0f;

		//Set the start widget index.
		_ActiveContainer->_StartWidgetIndex = _Widgets.Size();

		//Reset the cursor.
		_ActiveContainer->_Cursor = 0.0f;

		/*
		*	Add a scrollable widget to handle scrolling in this container.
		*	Important to add this beneath any incoming widgets, as those should consume the scroll input then.
		*/
		AddWidget<ScrollableWidget>(context, UI::Identifier("_SCROLLABLE"))
		->GetScrollableInterface()
		->SetUserData(_ActiveContainer)
		->SetOnScrollDownCallback
		(
			[](void *const RESTRICT user_data)
			{
				UI::Container *const RESTRICT container{ static_cast<UI::Container *const RESTRICT>(user_data) };

				if (!container->_WantsScroll)
				{
					return;
				}

				switch (container->_Layout)
				{
					case UI::Container::Layout::LEFT_TO_RIGHT:
					case UI::Container::Layout::RIGHT_TO_LEFT:
					case UI::Container::Layout::BOTTOM_TO_TOP:
					{
						ASSERT(false, "Implement with this with an actual use case!");
						//container->_ScrollOffset = BaseMath::Maximum<float32>(container->_ScrollOffset + container->_WidgetSize, 0.0f);

						break;
					}

					case UI::Container::Layout::TOP_TO_BOTTOM:
					{
						container->_ScrollOffset = BaseMath::Maximum<float32>(container->_ScrollOffset - container->_WidgetSize, -container->_MaximumScrollOffset);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		)
		->SetOnScrollUpCallback
		(
			[](void *const RESTRICT user_data)
			{
				UI::Container *const RESTRICT container{ static_cast<UI::Container *const RESTRICT>(user_data) };

				if (!container->_WantsScroll)
				{
					return;
				}

				switch (container->_Layout)
				{
					case UI::Container::Layout::LEFT_TO_RIGHT:
					case UI::Container::Layout::RIGHT_TO_LEFT:
					case UI::Container::Layout::BOTTOM_TO_TOP:
					{
						ASSERT(false, "Implement with this with an actual use case!");
						//container->_ScrollOffset = BaseMath::Minimum<float32>(container->_ScrollOffset - container->_WidgetSize, FLOAT32_MAXIMUM /*TODO!*/);

						break;
					}

					case UI::Container::Layout::TOP_TO_BOTTOM:
					{
						container->_ScrollOffset = BaseMath::Minimum<float32>(container->_ScrollOffset + container->_WidgetSize, 0.0f);

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		);
	}

	/*
	*	Sets the container layout.
	*	Note that this will reset the cursor.
	*/
	void Scene::SetContainerLayout(const UI::Container::Layout layout) NOEXCEPT
	{
		ASSERT(_ActiveContainer, "Trying to set widget size with no active container!");

		_ActiveContainer->_Layout = layout;
		_ActiveContainer->_Cursor = 0.0f;
	}

	/*
	*	Sets the container widget size.
	*	This is just a single value, because widgets by default take up all of one dimension, depending on layout;
	*	If the layout is horizontal, widgets will have [widget_size, container_size._Y] size,
	*	and if the layout is vertical, widgets will have [container_size._X, widget_size] size.
	*/
	void Scene::SetContainerWidgetSize(const float32 size) NOEXCEPT
	{
		ASSERT(_ActiveContainer, "Trying to set widget size with no active container!");

		_ActiveContainer->_WidgetSize = size;
	}

	/*
	*	Sets the container widget padding.
	*/
	void Scene::SetContainerWidgetPadding(const float32 padding) NOEXCEPT
	{
		ASSERT(_ActiveContainer, "Trying to set widget size with no active container!");

		_ActiveContainer->_WidgetPadding = padding;
	}

	/*
	*	Ends the active container.
	*/
	void Scene::EndContainer(const UI::BuildContext &context) NOEXCEPT
	{
		ASSERT(_ActiveContainer, "Trying to end a container while none is active!");

		//If the containers cursor has moved outside it's bounding box, then we need to add a scroll wheel and decide which widgets to render. (:
		bool is_cursor_inside;

		switch (_ActiveContainer->_Layout)
		{
			case UI::Container::Layout::LEFT_TO_RIGHT:
			{
				const Vector2<float32> cursor_position{ PreviousFloat32(_ActiveContainer->_Cursor), AxisAlignedBoundingBox2D::CalculateCenter(_ActiveContainer->_AxisAlignedBoundingBox)._Y };
				is_cursor_inside = _ActiveContainer->_AxisAlignedBoundingBox.IsInside(cursor_position);

				break;
			}

			case UI::Container::Layout::RIGHT_TO_LEFT:
			{
				const Vector2<float32> cursor_position{ _ActiveContainer->_AxisAlignedBoundingBox._Maximum._X - PreviousFloat32(_ActiveContainer->_Cursor), AxisAlignedBoundingBox2D::CalculateCenter(_ActiveContainer->_AxisAlignedBoundingBox)._Y };
				is_cursor_inside = _ActiveContainer->_AxisAlignedBoundingBox.IsInside(cursor_position);

				break;
			}

			case UI::Container::Layout::TOP_TO_BOTTOM:
			{
				if (_ActiveContainer->_Cursor > 0.0f)
				{
					const Vector2<float32> cursor_position{ AxisAlignedBoundingBox2D::CalculateCenter(_ActiveContainer->_AxisAlignedBoundingBox)._X, NextFloat32(_ActiveContainer->_AxisAlignedBoundingBox._Maximum._Y - _ActiveContainer->_Cursor) };
					is_cursor_inside = _ActiveContainer->_AxisAlignedBoundingBox.IsInside(cursor_position);
				}
				
				else
				{
					is_cursor_inside = true;
				}

				break;
			}

			case UI::Container::Layout::BOTTOM_TO_TOP:
			{
				const Vector2<float32> cursor_position{ AxisAlignedBoundingBox2D::CalculateCenter(_ActiveContainer->_AxisAlignedBoundingBox)._X, PreviousFloat32(_ActiveContainer->_Cursor) };
				is_cursor_inside = _ActiveContainer->_AxisAlignedBoundingBox.IsInside(cursor_position);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//If the cursor is not inside, go and choose if widgets should be enabled depending on if they are fully inside the container.
		if (!is_cursor_inside)
		{
			for (uint64 widget_index{ _ActiveContainer->_StartWidgetIndex }; widget_index < _Widgets.Size(); ++widget_index)
			{
				UI::Widget *const RESTRICT widget{ _Widgets[widget_index] };
				AxisAlignedBoundingBox2D widget_axis_aligned_bounding_box{ widget->GetAxisAlignedBoundingBox() };

				bool is_fully_inside{ true };

				is_fully_inside &= widget_axis_aligned_bounding_box._Minimum._X >= _ActiveContainer->_AxisAlignedBoundingBox._Minimum._X;
				is_fully_inside &= widget_axis_aligned_bounding_box._Maximum._X <= _ActiveContainer->_AxisAlignedBoundingBox._Maximum._X;
				is_fully_inside &= widget_axis_aligned_bounding_box._Minimum._Y >= _ActiveContainer->_AxisAlignedBoundingBox._Minimum._Y;
				is_fully_inside &= widget_axis_aligned_bounding_box._Maximum._Y <= _ActiveContainer->_AxisAlignedBoundingBox._Maximum._Y;

				if (is_fully_inside)
				{
					//Modify the widget's axis aligned bounding box a bit to account for the incoming scroll bar.
					switch (_ActiveContainer->_Layout)
					{
						case UI::Container::Layout::LEFT_TO_RIGHT:
						case UI::Container::Layout::RIGHT_TO_LEFT:
						{
							widget_axis_aligned_bounding_box._Minimum._Y += ScrollBarWidget::SIZE;

							break;
						}

						case UI::Container::Layout::TOP_TO_BOTTOM:
						case UI::Container::Layout::BOTTOM_TO_TOP:
						{
							widget_axis_aligned_bounding_box._Maximum._X -= ScrollBarWidget::SIZE;

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}

					widget->SetAxisAlignedBoundingBox(widget_axis_aligned_bounding_box);

					widget->SetEnabled(true);
				}
				
				else
				{
					widget->SetEnabled(false);
				}
			}

			//Calculate the start/end for the scroll bar.
			float32 scroll_bar_start;
			float32 scroll_bar_end;

			switch (_ActiveContainer->_Layout)
			{
				case UI::Container::Layout::LEFT_TO_RIGHT:
				case UI::Container::Layout::RIGHT_TO_LEFT:
				{
					ASSERT(false, "Implement this with an actual use case!");

					/*
					const float32 width{ _ActiveContainer->_AxisAlignedBoundingBox.Dimensions()._X };
					scroll_bar_start = 0.0f;
					scroll_bar_end = width / _ActiveContainer->_Cursor;
					*/

					break;
				}

				case UI::Container::Layout::TOP_TO_BOTTOM:
				{
					const float32 height{ _ActiveContainer->_AxisAlignedBoundingBox.Dimensions()._Y };

					scroll_bar_start = (_ActiveContainer->_Cursor + _ActiveContainer->_ScrollOffset - height) / _ActiveContainer->_Cursor;
					scroll_bar_end = (_ActiveContainer->_Cursor + _ActiveContainer->_ScrollOffset) / _ActiveContainer->_Cursor;

					_ActiveContainer->_MaximumScrollOffset = _ActiveContainer->_Cursor - height;

					break;
				}

				case UI::Container::Layout::BOTTOM_TO_TOP:
				{
					ASSERT(false, "Implement this with an actual use case!");

					/*
					const float32 height{ _ActiveContainer->_AxisAlignedBoundingBox.Dimensions()._Y };
					scroll_bar_start = 0.0f;
					scroll_bar_end = height / _ActiveContainer->_Cursor;
					*/

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			//Now add the scroll bar widget!
			AddWidget<UI::ScrollBarWidget>(context, UI::Identifier("_SCROLL_BAR"))->SetRange(scroll_bar_start, scroll_bar_end);
		}

		//Otherwise, enable all widgets.
		else
		{
			for (uint64 widget_index{ _ActiveContainer->_StartWidgetIndex }; widget_index < _Widgets.Size(); ++widget_index)
			{
				UI::Widget *const RESTRICT widget{ _Widgets[widget_index] };

				widget->SetEnabled(true);
			}
		}

		//Set whether or not this container wants scroll.
		_ActiveContainer->_WantsScroll = !is_cursor_inside;

		//Set the end widget index.
		_ActiveContainer->_EndWidgetIndex = _Widgets.Size();

		//Reset the active container.
		_ActiveContainer = nullptr;
	}

}