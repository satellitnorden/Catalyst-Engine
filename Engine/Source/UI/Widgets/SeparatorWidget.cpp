//Header file.
#include <UI/Widgets/SeparatorWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void SeparatorWidget::Position() NOEXCEPT
	{
		switch (_Parent->_Layout)
		{
			case UI::Container::Layout::LEFT_TO_RIGHT:
			case UI::Container::Layout::RIGHT_TO_LEFT:
			case UI::Container::Layout::BOTTOM_TO_TOP:
			{
				ASSERT(false, "Implement this when we have an actual use case. (:");

				break;
			}

			case UI::Container::Layout::TOP_TO_BOTTOM:
			{
				_AxisAlignedBoundingBox._Minimum = Vector2<float32>
				(
					_Parent->_AxisAlignedBoundingBox._Minimum._X + _Parent->_WidgetPadding,
					_Parent->_AxisAlignedBoundingBox._Maximum._Y - (_Parent->_Cursor + _Parent->_ScrollOffset) - (_Parent->_WidgetPadding * 0.25f)
				);
				_AxisAlignedBoundingBox._Maximum = Vector2<float32>
				(
					_Parent->_AxisAlignedBoundingBox._Maximum._X - _Parent->_WidgetPadding,
					_Parent->_AxisAlignedBoundingBox._Maximum._Y - (_Parent->_Cursor + _Parent->_ScrollOffset) + (_Parent->_WidgetPadding * 0.25f)
				);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Renders this widget.
	*/
	void SeparatorWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Render the box.
		RenderBox(context, _AxisAlignedBoundingBox, _Parent->_Parent->GetStyle()._UncheckedHoveredColor, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), _Parent->_Parent->GetStyle()._Rounding);
	}

}