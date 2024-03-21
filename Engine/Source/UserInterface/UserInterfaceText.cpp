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
										const AssetPointer<FontAsset> font,
										const float32 scale,
										const TextHorizontalAlignment horizontal_alignment,
										const TextVerticalAlignment vertical_alignment,
										const bool is_three_dimensional) NOEXCEPT
{
	//Set the type.
	SetType(UserInterfaceElementType::TEXT);

	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the font.
	_Font = font;

	//Set the scale.
	_Scale = scale;

	//Set the horizontal alignment.
	_HorizontalAlignment = horizontal_alignment;

	//Set the vertical alignment.
	_VerticalAlignment = vertical_alignment;

	//Remember whether or not this text is three dimensional.
	_IsThreeDimensional = is_three_dimensional;

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
*	Sets the position.
*/
void UserInterfaceText::SetPosition(const Vector2<float32>& position) NOEXCEPT
{
	const Vector2<float32> half_extents{ (_Maximum - _Minimum) * 0.5f };

	_Minimum = position - half_extents;
	_Maximum = position + half_extents;

	if (_Primitive)
	{
		_Primitive->_Minimum = _Minimum;
		_Primitive->_Maximum = _Maximum;
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
			description._Font = _Font;
			description._Scale = _Scale;
			description._HorizontalAlignment = _HorizontalAlignment;
			description._VerticalAlignment = _VerticalAlignment;
			description._Text = text;

			_Primitive = static_cast<TextUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description, _IsThreeDimensional));
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

/*
*	Sets the opacity.
*/
void UserInterfaceText::SetOpacity(const float32 opacity) NOEXCEPT
{
	if (_Primitive)
	{
		_Primitive->_Opacity = opacity;
	}
}

/*
*	Returns the user interface primitives.
*/
void UserInterfaceText::RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	if (_Primitive)
	{
		output->Emplace(_Primitive);
	}
}