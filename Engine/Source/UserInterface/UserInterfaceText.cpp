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
										const ResourcePointer<FontResource> font_resource,
										const float32 scale,
										const TextHorizontalAlignment horizontal_alignment,
										const TextVerticalAlignment vertical_alignment,
										const float32 smoothing_factor,
										const bool is_three_dimensional) NOEXCEPT
{
	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the font resource.
	_FontResource = font_resource;

	//Set the scale.
	_Scale = scale;

	//Set the horizontal alignment.
	_HorizontalAlignment = horizontal_alignment;

	//Set the vertical alignment.
	_VerticalAlignment = vertical_alignment;

	//Set the smoothing factor.
	_SmoothingFactor = smoothing_factor;

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
			description._FontResource = _FontResource;
			description._Scale = _Scale;
			description._HorizontalAlignment = _HorizontalAlignment;
			description._VerticalAlignment = _VerticalAlignment;
			description._TextSmoothingFactor = _SmoothingFactor;
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