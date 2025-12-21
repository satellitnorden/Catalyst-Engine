//Header file.
#include <UI/Widgets/ScrollBarWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	//Scroll bar widget constants.
	namespace ScrollBarWidgetConstants
	{
		constexpr Vector4<float32> BACKGROUND_COLOR{ 0.0625f, 0.0625f, 0.0625f, 0.125f };
		constexpr Vector4<float32> HANDLE_COLOR{ 0.25f, 0.25f, 0.25f, 0.5f };
	}

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void ScrollBarWidget::Position() NOEXCEPT
	{
		switch (_Parent->_Layout)
		{
			case UI::Container::Layout::LEFT_TO_RIGHT:
			case UI::Container::Layout::RIGHT_TO_LEFT:
			{
				_AxisAlignedBoundingBox._Minimum._X = _Parent->_AxisAlignedBoundingBox._Minimum._X + _Parent->_WidgetPadding;
				_AxisAlignedBoundingBox._Minimum._Y = _Parent->_AxisAlignedBoundingBox._Minimum._Y;

				_AxisAlignedBoundingBox._Maximum._X = _Parent->_AxisAlignedBoundingBox._Maximum._X - _Parent->_WidgetPadding;
				_AxisAlignedBoundingBox._Maximum._Y = _Parent->_AxisAlignedBoundingBox._Minimum._Y + SIZE;

				break;
			}

			case UI::Container::Layout::TOP_TO_BOTTOM:
			case UI::Container::Layout::BOTTOM_TO_TOP:
			{
				_AxisAlignedBoundingBox._Minimum._X = _Parent->_AxisAlignedBoundingBox._Maximum._X - SIZE;
				_AxisAlignedBoundingBox._Minimum._Y = _Parent->_AxisAlignedBoundingBox._Minimum._Y + _Parent->_WidgetPadding;

				_AxisAlignedBoundingBox._Maximum._X = _Parent->_AxisAlignedBoundingBox._Maximum._X;
				_AxisAlignedBoundingBox._Maximum._Y = _Parent->_AxisAlignedBoundingBox._Maximum._Y - _Parent->_WidgetPadding;

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
	void ScrollBarWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Render the background box.
		RenderBox(context, _AxisAlignedBoundingBox, ScrollBarWidgetConstants::BACKGROUND_COLOR, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), SIZE * 0.5f);

		//Add the handle box.
		{
			AxisAlignedBoundingBox2D axis_aligned_bounding_box{ _AxisAlignedBoundingBox };

			axis_aligned_bounding_box._Minimum._Y = BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._Y, _AxisAlignedBoundingBox._Maximum._Y, _Start);
			axis_aligned_bounding_box._Maximum._Y = BaseMath::LinearlyInterpolate(_AxisAlignedBoundingBox._Minimum._Y, _AxisAlignedBoundingBox._Maximum._Y, _End);

			RenderBox(context, axis_aligned_bounding_box, ScrollBarWidgetConstants::HANDLE_COLOR, Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f), SIZE * 0.5f);
		}
	}

}