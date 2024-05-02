//Header file.
#include <UserInterface/UserInterfaceButton.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

/*
*	Default constructor.
*/
UserInterfaceButton::UserInterfaceButton(	const Vector2<float32> initial_minimum,
											const Vector2<float32> initial_maximum,
											const Callback initial_start_pressed_callback,
											const Callback initial_stop_pressed_callback,
											const Callback initial_start_hovered_callback,
											const Callback initial_stop_hovered_callback,
											const UserInterfaceMaterial &initial_idle_material,
											const UserInterfaceMaterial &initial_hovered_material,
											const UserInterfaceMaterial &initial_pressed_material,
											const char *const RESTRICT text,
											const AssetPointer<FontAsset> font,
											const float32 text_scale) NOEXCEPT
{
	//Set the type.
	SetType(UserInterfaceElementType::BUTTON);

	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the current state.
	_CurrentState = UserInterfaceButtonState::IDLE;
	
	//Set the callbacks.
	_StartPressedCallback = initial_start_pressed_callback;
	_StopPressedCallback = initial_stop_pressed_callback;
	_StartHoveredCallback = initial_start_hovered_callback;
	_StopHoveredCallback = initial_stop_hovered_callback;

	//Set the materials.
	_IdleMaterial = initial_idle_material;
	_HoveredMaterial = initial_hovered_material;
	_PressedMaterial = initial_pressed_material;

	//Set the font.
	_Font = font;

	//Set the text scale.
	_TextScale = text_scale;

	//Add the image Primitive.
	{
		ImageUserInterfacePrimitiveDescription description;

		description._Type = UserInterfacePrimitiveType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = 1.0f;
		description._Material = initial_idle_material;

		_ImagePrimitive = static_cast<ImageUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
	}

	//Set the text.
	SetText(text);
}

/*
*	Default destructor.
*/
UserInterfaceButton::~UserInterfaceButton() NOEXCEPT
{
	//Destroy the primitives.
	UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_ImagePrimitive);

	if (_TextPrimitive)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TextPrimitive);
	}
}

/*
*	Sets the current state.
*/
void UserInterfaceButton::SetCurrentState(const UserInterfaceButtonState value) NOEXCEPT
{
	_CurrentState = value;

	switch (_CurrentState)
	{
		case UserInterfaceButtonState::IDLE:
		{
			_ImagePrimitive->_Material = _IdleMaterial;

			break;
		}

		case UserInterfaceButtonState::HOVERED:
		{
			_ImagePrimitive->_Material = _HoveredMaterial;

			break;
		}

		case UserInterfaceButtonState::PRESSED:
		{
			_ImagePrimitive->_Material = _PressedMaterial;

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
*	Sets the idle material.
*/
void UserInterfaceButton::SetIdleMaterial(const UserInterfaceMaterial &value) NOEXCEPT
{
	_IdleMaterial = value;

	if (_CurrentState == UserInterfaceButtonState::IDLE)
	{
		_ImagePrimitive->_Material = _IdleMaterial;
	}
}

/*
*	Sets the hovered material.
*/
void UserInterfaceButton::SetHoveredMaterial(const UserInterfaceMaterial &value) NOEXCEPT
{
	_HoveredMaterial = value;

	if (_CurrentState == UserInterfaceButtonState::HOVERED)
	{
		_ImagePrimitive->_Material = _HoveredMaterial;
	}
}

/*
*	Sets the pressed material.
*/
void UserInterfaceButton::SetPressedMaterial(const UserInterfaceMaterial &value) NOEXCEPT
{
	_PressedMaterial = value;

	if (_CurrentState == UserInterfaceButtonState::PRESSED)
	{
		_ImagePrimitive->_Material = _PressedMaterial;
	}
}

/*
*	Sets the text.
*/
void UserInterfaceButton::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_TextPrimitive)
		{
			TextUserInterfacePrimitiveDescription description;

			description._Type = UserInterfacePrimitiveType::TEXT;
			description._Minimum = _Minimum;
			description._Maximum = _Maximum;
			description._Opacity = 1.0f;
			description._Font = _Font;
			description._Scale = _TextScale;
			description._HorizontalAlignment = TextHorizontalAlignment::CENTER;
			description._VerticalAlignment = TextVerticalAlignment::CENTER;
			description._Text = text;

			_TextPrimitive = static_cast<TextUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
		}

		else
		{
			_TextPrimitive->_Text = text;
		}
	}

	else
	{
		if (_TextPrimitive)
		{
			UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TextPrimitive);
		}

		_TextPrimitive = nullptr;
	}
}

/*
*	Sets the opacity.
*/
void UserInterfaceButton::SetOpacity(const float32 value) NOEXCEPT
{
	_ImagePrimitive->_Opacity = value;

	if (_TextPrimitive)
	{
		_TextPrimitive->_Opacity = value;
	}
}

/*
*	Returns the user interface primitives.
*/
void UserInterfaceButton::RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_ImagePrimitive);
	
	if (_TextPrimitive)
	{
		output->Emplace(_TextPrimitive);
	}
}