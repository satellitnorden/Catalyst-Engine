//Header file.
#include <UI/Core/Scene.h>

//Systems.
#include <Systems/ContentSystem.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	Scene::Scene() NOEXCEPT
	{
		//Set the default font.
		_Font = ContentSystem::Instance->GetAsset<FontAsset>(HashString("Default"));

		//Set the default text scale.
		_TextScale = 32.0f;
	}

	/*
	*	Starts a container with the given identifier.
	*/
	void Scene::StartContainer(const UI::Identifier identifier, const UI::Anchor anchor, const UI::Container::Layout layout, const AxisAlignedBoundingBox2D &axis_aligned_bounding_box) NOEXCEPT
	{
		ASSERT(!_Container._Active, "Trying to start a container while another is active!");

		//Set the parent.
		_Container._Parent = this;

		//Copy settings.
		_Container._Identifier = identifier;
		_Container._Anchor = anchor;
		_Container._Layout = layout;

		//Set up the axis aligned bounding box.
		{
			//Calculate commonly used values.
			const Vector2<float32> center{ BaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum, axis_aligned_bounding_box._Maximum, 0.5f) };
			const Vector2<float32> size{ axis_aligned_bounding_box._Maximum - axis_aligned_bounding_box._Minimum };
			const Vector2<float32> half_size{ size * 0.5f };

			//Set up the bounding box differently depending on the anchor.
			switch (anchor)
			{
				case UI::Anchor::TOP:
				{
					_Container._AxisAlignedBoundingBox._Minimum._X = center._X - half_size._X;
					_Container._AxisAlignedBoundingBox._Minimum._Y = UI::Constants::REFERENCE_RESOLUTION._Y - size._Y;

					_Container._AxisAlignedBoundingBox._Maximum._X = center._X + half_size._X;
					_Container._AxisAlignedBoundingBox._Maximum._Y = UI::Constants::REFERENCE_RESOLUTION._Y;

					break;
				}

				case UI::Anchor::BOTTOM_LEFT:
				{
					_Container._AxisAlignedBoundingBox._Minimum._X = 0.0f;
					_Container._AxisAlignedBoundingBox._Minimum._Y = 0.0f;

					_Container._AxisAlignedBoundingBox._Maximum._X = size._X;
					_Container._AxisAlignedBoundingBox._Maximum._Y = size._Y;

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
				_Container._WidgetSize = UI::Constants::REFERENCE_RESOLUTION._X * 0.1f;

				break;
			}

			case UI::Container::Layout::TOP_TO_BOTTOM:
			case UI::Container::Layout::BOTTOM_TO_TOP:
			{
				_Container._WidgetSize = UI::Constants::REFERENCE_RESOLUTION._Y * 0.1f;

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}

		//Set up the (default) widget padding.
		_Container._WidgetPadding = 8.0f;

		//Reset the cursor.
		_Container._Cursor = 0.0f;

		//The container is now active!
		_Container._Active = true;
	}

	/*
	*	Sets the container layout.
	*	Note that this will reset the cursor.
	*/
	void Scene::SetContainerLayout(const UI::Container::Layout layout) NOEXCEPT
	{
		ASSERT(_Container._Active, "Trying to set widget size with no active container!");

		_Container._Layout = layout;
		_Container._Cursor = 0.0f;
	}

	/*
	*	Sets the container widget size.
	*	This is just a single value, because widgets by default take up all of one dimension, depending on layout;
	*	If the layout is horizontal, widgets will have [widget_size, container_size._Y] size,
	*	and if the layout is vertical, widgets will have [container_size._X, widget_size] size.
	*/
	void Scene::SetContainerWidgetSize(const float32 size) NOEXCEPT
	{
		ASSERT(_Container._Active, "Trying to set widget size with no active container!");

		_Container._WidgetSize = size;
	}

	/*
	*	Sets the container widget padding.
	*/
	void Scene::SetContainerWidgetPadding(const float32 padding) NOEXCEPT
	{
		ASSERT(_Container._Active, "Trying to set widget size with no active container!");

		_Container._WidgetPadding = padding;
	}

	/*
	*	Ends the active container.
	*/
	void Scene::EndContainer() NOEXCEPT
	{
		ASSERT(_Container._Active, "Trying to end a container while none is active!");

		//The container is no longer active.
		_Container._Active = false;
	}

}