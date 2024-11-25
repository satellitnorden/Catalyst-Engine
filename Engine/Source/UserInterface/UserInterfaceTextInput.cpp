//Header file.
#include <UserInterface/UserInterfaceTextInput.h>

//Systems
#include <Systems/InputSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

//The active text input. There can be only one. (:
UserInterfaceTextInput *RESTRICT ACTIVE_TEXT_INPUT = nullptr;

/*
*	Default constructor.
*/
UserInterfaceTextInput::UserInterfaceTextInput(	const Vector2<float32> initial_minimum,
												const Vector2<float32> initial_maximum,
												const UserInterfaceMaterial &initial_idle_material,
												const UserInterfaceMaterial &initial_active_material,
												const UserInterfaceMaterial &initial_hovered_material,
												const UserInterfaceMaterial &initial_pressed_material,
												const AssetPointer<FontAsset> font,
												const float32 text_scale,
												const char *const RESTRICT prompt_text,
												const char *const RESTRICT text,
												const Callback callback) NOEXCEPT
{
	//Set the type.
	SetType(UserInterfaceElementType::TEXT_INPUT);

	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the materials.
	_IdleMaterial = initial_idle_material;
	_ActiveMaterial = initial_active_material;
	_HoveredMaterial = initial_hovered_material;
	_PressedMaterial = initial_pressed_material;

	//Set the font.
	_Font = font;

	//Set the text scale.
	_TextScale = text_scale;

	//Set the prompt text.
	_PromptText = prompt_text;

	//Set the text.
	if (text)
	{
		_CurrentText = text;
	}

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

	//Set the callback.
	_Callback = callback;

	//Set the text.
	if (_CurrentText.Length() > 0)
	{
		SetText(_CurrentText.Data());
		SetTextOpacity(1.0f);
	}

	else
	{
		SetText(_PromptText.Data());
		SetTextOpacity(0.5f);
	}
}

/*
*	Default destructor.
*/
UserInterfaceTextInput::~UserInterfaceTextInput() NOEXCEPT
{
	//Destroy the primitives.
	UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_ImagePrimitive);

	if (_TextPrimitive)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TextPrimitive);
	}

	//If this was the active text input, reset.
	if (ACTIVE_TEXT_INPUT == this)
	{
		ACTIVE_TEXT_INPUT = nullptr;
	}
}

/*
*	Updates this user interface element.
*/
void UserInterfaceTextInput::Update() NOEXCEPT
{
	//Update the material.
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState(InputLayer::USER_INTERFACE) };

	bool is_hovered{ false };

	{
		AxisAlignedBoundingBox2D bounding_box{ _Minimum, _Maximum };

		is_hovered = bounding_box.IsInside(Vector2<float32>(mouse_state->_CurrentX, mouse_state->_CurrentY));
	}

	//If the mouse was pressed while this is hovered, make this text input the active one.
	if (is_hovered && mouse_state->_Left == ButtonState::PRESSED)
	{
		ACTIVE_TEXT_INPUT = this;
	}

	if (is_hovered)
	{
		if (ACTIVE_TEXT_INPUT == this && (mouse_state->_Left == ButtonState::PRESSED || mouse_state->_Left == ButtonState::PRESSED_HELD))
		{
			_ImagePrimitive->_Material = _PressedMaterial;
		}

		else
		{
			_ImagePrimitive->_Material = _HoveredMaterial;
		}
	}

	else
	{
		if (ACTIVE_TEXT_INPUT == this)
		{
			_ImagePrimitive->_Material = _ActiveMaterial;
		}

		else
		{
			_ImagePrimitive->_Material = _IdleMaterial;
		}
	}

	//The rest is only for active text inputs.
	if (ACTIVE_TEXT_INPUT != this)
	{
		return;
	}

	//Remember if the text was updated.
	bool text_was_updated{ false };

	//Retrieve the keyboard state.
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState(InputLayer::GAME) };

	//Add characters.
	for (uint64 i{ 0 }; i < CatalystPlatform::NumberOfInputCharacters(); ++i)
	{
		const char new_character{ CatalystPlatform::InputCharacterAt(i) };

		if (new_character == '\b')
		{
			continue;
		}

		_CurrentText += new_character;
		text_was_updated = true;
	}

	//Check removal of characters.
	if (_CurrentText.Length() > 0 && keyboard_state->GetButtonState(KeyboardButton::Backspace) == ButtonState::PRESSED)
	{
		_CurrentText.SetLength(_CurrentText.Length() - 1);
		text_was_updated = true;
	}

	if (text_was_updated)
	{
		if (_CurrentText.Length() > 0)
		{
			SetText(_CurrentText.Data());
			SetTextOpacity(1.0f);
		}

		else
		{
			SetText(_PromptText.Data());
			SetTextOpacity(0.5f);
		}
		
		if (_Callback)
		{
			_Callback(this);
		}
	}
}

/*
*	Sets the idle material.
*/
void UserInterfaceTextInput::SetIdleMaterial(const UserInterfaceMaterial &value) NOEXCEPT
{
	_IdleMaterial = value;
	_ImagePrimitive->_Material = _IdleMaterial;
}

/*
*	Sets the text.
*/
void UserInterfaceTextInput::SetText(const char *const RESTRICT text) NOEXCEPT
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
			description._HorizontalAlignment = TextHorizontalAlignment::LEFT;
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
*	Sets the text opacity.
*/
void UserInterfaceTextInput::SetTextOpacity(const float32 value) NOEXCEPT
{
	if (_TextPrimitive)
	{
		_TextPrimitive->_Opacity = value;
	}
}

/*
*	Returns the user interface primitives.
*/
void UserInterfaceTextInput::RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_ImagePrimitive);
	
	if (_TextPrimitive)
	{
		output->Emplace(_TextPrimitive);
	}
}