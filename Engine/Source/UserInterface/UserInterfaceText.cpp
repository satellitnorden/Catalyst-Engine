//Header file.
#include <UserInterface/UserInterfaceText.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/TextUserInterfaceElementDescription.h>

/*
*	Default constructor.
*/
UserInterfaceText::UserInterfaceText(	const Vector2<float32> initial_minimum,
										const Vector2<float32> initial_maximum,
										const char *const RESTRICT text,
										const TextHorizontalAlignment horizontal_alignment) NOEXCEPT
{
	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the horizontal alignment.
	_HorizontalAlignment = horizontal_alignment;

	//Set the text.
	SetText(text);
}

/*
*	Default destructor.
*/
UserInterfaceText::~UserInterfaceText() NOEXCEPT
{
	//Destroy the element.
	if (_Element)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_Element);
	}
}

/*
*	Sets the text.
*/
void UserInterfaceText::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_Element)
		{
			TextUserInterfaceElementDescription description;

			description._Type = UserInterfaceElementType::TEXT;
			description._Minimum = _Minimum;
			description._Maximum = _Maximum;
			description._Opacity = 1.0f;
			description._FontResource = ResourceSystem::Instance->GetFontResource(HashString("Catalyst_Engine_Default_Font"));
			description._Scale = 0.015f;
			description._HorizontalAlignment = _HorizontalAlignment;
			description._VerticalAlignment = TextVerticalAlignment::CENTER;
			description._TextSmoothingFactor = 0.2f;
			description._Text = text;

			_Element = static_cast<TextUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
		}

		else
		{
			_Element->_Text = text;
		}
	}

	else
	{
		if (_Element)
		{
			UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_Element);
		}

		_Element = nullptr;
	}
}