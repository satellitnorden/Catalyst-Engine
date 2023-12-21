//Header file.
#include <UserInterface/UserInterfaceTextInput.h>

//Systems
#include <Systems/InputSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

/*
*	Default constructor.
*/
UserInterfaceTextInput::UserInterfaceTextInput(	const Vector2<float32> initial_minimum,
												const Vector2<float32> initial_maximum,
												const UserInterfaceMaterial& initial_idle_material,
												const ResourcePointer<FontResource> font_resource,
												const float32 text_scale,
												const char* const RESTRICT prompt_text,
												const char* const RESTRICT text) NOEXCEPT
{
	//Set the type.
	SetType(UserInterfaceElementType::TEXT_INPUT);

	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the materials.
	_IdleMaterial = initial_idle_material;

	//Set the font resource.
	_FontResource = font_resource;

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

		_ImagePrimitive = static_cast<ImageUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description, false));
	}

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
}

/*
*	Updates this user interface element.
*/
void UserInterfaceTextInput::Update() NOEXCEPT
{
	//Remember if the text was updated.
	bool text_was_updated{ false };

	//Retrieve the keyboard state.
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState(InputLayer::GAME) };

	//Check if shift is held down.
	const bool shift_held
	{
		keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED
		|| keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED_HELD
		|| keyboard_state->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED
		|| keyboard_state->GetButtonState(KeyboardButton::RightShift) == ButtonState::PRESSED_HELD
	};

	//Add characters.
	for (uint32 i{ 0 }; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
	{
		const ButtonState button_state{ keyboard_state->GetButtonState(static_cast<KeyboardButton>(i)) };
		const char upper_case_character{ keyboard_state->GetUpperCaseCharacter(static_cast<KeyboardButton>(i)) };
		const char lower_case_character{ keyboard_state->GetLowerCaseCharacter(static_cast<KeyboardButton>(i)) };

		if (button_state == ButtonState::PRESSED && upper_case_character != CHAR_MAX && lower_case_character != CHAR_MAX)
		{
			_CurrentText += shift_held ? upper_case_character : lower_case_character;
			text_was_updated = true;
		}
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
			description._FontResource = _FontResource;
			description._Scale = _TextScale;
			description._HorizontalAlignment = TextHorizontalAlignment::CENTER;
			description._VerticalAlignment = TextVerticalAlignment::CENTER;
			description._Text = text;

			_TextPrimitive = static_cast<TextUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description, false));
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
void UserInterfaceTextInput::RetrieveUserInterfacePrimitives(DynamicArray<const UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_ImagePrimitive);
	
	if (_TextPrimitive)
	{
		output->Emplace(_TextPrimitive);
	}
}