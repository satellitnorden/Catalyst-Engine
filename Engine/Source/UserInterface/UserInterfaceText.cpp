//Header file.
#include <UserInterface/UserInterfaceText.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

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
	//Destroy the primitive.
	if (_Primitive)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_Primitive);
	}
}

/*
*	Sets the text.
*/
void UserInterfaceText::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_Primitive)
		{
			TextUserInterfacePrimitiveDescription description;

			description._Type = UserInterfacePrimitiveType::TEXT;
			description._Minimum = _Minimum;
			description._Maximum = _Maximum;
			description._Opacity = 1.0f;
			description._FontResource = ResourceSystem::Instance->GetFontResource(HashString("Catalyst_Engine_Default_Font"));
			description._Scale = 0.015f;
			description._HorizontalAlignment = _HorizontalAlignment;
			description._VerticalAlignment = TextVerticalAlignment::CENTER;
			description._TextSmoothingFactor = 0.2f;
			description._Text = text;

			_Primitive = static_cast<TextUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
		}

		else
		{
			_Primitive->_Text = text;
		}
	}

	else
	{
		if (_Primitive)
		{
			UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_Primitive);
		}

		_Primitive = nullptr;
	}
}