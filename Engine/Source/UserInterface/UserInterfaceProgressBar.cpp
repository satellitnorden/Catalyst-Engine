//Header file.
#include <UserInterface/UserInterfaceProgressBar.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElementDescription.h>
#include <UserInterface/TextUserInterfaceElementDescription.h>

/*
*	Default constructor.
*/
UserInterfaceProgressBar::UserInterfaceProgressBar(	const Vector2<float32> initial_minimum,
													const Vector2<float32> initial_maximum,
													const UserInterfaceMaterial &initial_bottom_material,
													const UserInterfaceMaterial &initial_top_material,
													const char *const RESTRICT text) NOEXCEPT
{
	//Create the bottom and top element.
	{
		ImageUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = 1.0f;
		description._Material = initial_bottom_material;

		_BottomElement = static_cast<ImageUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}

	{
		ImageUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = Vector2<float32>(initial_minimum._X, initial_maximum._Y);
		description._Opacity = 1.0f;
		description._Material = initial_top_material;

		_TopElement = static_cast<ImageUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}

	//Set the text.
	SetText(text);
}

/*
*	Default destructor.
*/
UserInterfaceProgressBar::~UserInterfaceProgressBar() NOEXCEPT
{
	//Destroy the elements.
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_BottomElement);
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_TopElement);

	if (_TextElement)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_TextElement);
	}
}

/*
*	Sets the current progress.
*/
void UserInterfaceProgressBar::SetCurrentProgress(const float32 value) NOEXCEPT
{
	//Set the current progress.
	_CurrentProgress = value;

	//Update the bounding box for the top element.
	_TopElement->_Maximum._X = CatalystBaseMath::LinearlyInterpolate(_BottomElement->_Minimum._X, _BottomElement->_Maximum._X, _CurrentProgress);
}

/*
*	Sets the text.
*/
void UserInterfaceProgressBar::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_TextElement)
		{
			TextUserInterfaceElementDescription description;

			description._Type = UserInterfaceElementType::TEXT;
			description._Minimum = _BottomElement->_Minimum;
			description._Maximum = _BottomElement->_Maximum;
			description._Opacity = 1.0f;
			description._FontResource = ResourceSystem::Instance->GetFontResource(HashString("Catalyst_Engine_Default_Font"));
			description._Scale = 0.015f;
			description._HorizontalAlignment = TextHorizontalAlignment::CENTER;
			description._VerticalAlignment = TextVerticalAlignment::CENTER;
			description._TextSmoothingFactor = 0.2f;
			description._Text = text;

			_TextElement = static_cast<TextUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
		}

		else
		{
			_TextElement->_Text = text;
		}
	}

	else
	{
		if (_TextElement)
		{
			UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_TextElement);
		}

		_TextElement = nullptr;
	}
}