//Header file.
#include <UserInterface/UserInterfaceCheckbox.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

/*
*	Default constructor.
*/
UserInterfaceCheckbox::UserInterfaceCheckbox(	const Vector2<float32> initial_minimum,
												const Vector2<float32> initial_maximum,
												const bool initially_checked,
												const Callback initial_start_hovered_callback,
												const Callback initial_stop_hovered_callback,
												const Callback initial_start_pressed_callback,
												const Callback initial_stop_pressed_callback,
												const UserInterfaceMaterial &initial_unchecked_idle_material,
												const UserInterfaceMaterial &initial_unchecked_hovered_material,
												const UserInterfaceMaterial &initial_unchecked_pressed_material,
												const UserInterfaceMaterial &initial_checked_idle_material,
												const UserInterfaceMaterial &initial_checked_hovered_material,
												const UserInterfaceMaterial &initial_checked_pressed_material,
												const char *const RESTRICT text,
												const float32 text_scale,
												const TextHorizontalAlignment horizontal_alignment,
												const TextVerticalAlignment vertical_alignment,
												const AssetPointer<FontAsset> font) NOEXCEPT
{
	//Set the type.
	SetType(UserInterfaceElementType::CHECKBOX);

	//Calculate the initial extent.
	const Vector2<float32> initial_extent{ initial_maximum - initial_minimum };

	//Calculate the button minimum/maximum.
	_ButtonMinimum = initial_minimum;
	_ButtonMaximum = initial_minimum + Vector2<float32>(initial_extent._Y, initial_extent._Y);

	//Calculate the button width.
	const float32 button_width{ _ButtonMaximum._X - _ButtonMinimum._X };

	//Cache the aspect ratio.
	const float32 aspect_ratio{ RenderingSystem::Instance->GetFullAspectRatio() };

	//Apply the aspect ratio.
	_ButtonMaximum._X = _ButtonMinimum._X + (button_width / aspect_ratio);
	 
	//Calculate the text minimum/maximum.
	_TextMinimum = Vector2<float32>(_ButtonMaximum._X, _ButtonMinimum._Y);
	_TextMaximum = initial_maximum;

	//Set whether or not this checkbox is initially checked.
	_IsChecked = initially_checked;

	//Set the current state.
	_CurrentState = UserInterfaceButtonState::IDLE;
	
	//Set the callbacks.
	_StartHoveredCallback = initial_start_hovered_callback;
	_StopHoveredCallback = initial_stop_hovered_callback;
	_StartPressedCallback = initial_start_pressed_callback;
	_StopPressedCallback = initial_stop_pressed_callback;

	//Set the materials.
	_UncheckedIdleMaterial = initial_unchecked_idle_material;
	_UncheckedHoveredMaterial = initial_unchecked_hovered_material;
	_UncheckedPressedMaterial = initial_unchecked_pressed_material;
	_CheckedIdleMaterial = initial_checked_idle_material;
	_CheckedHoveredMaterial = initial_checked_hovered_material;
	_CheckedPressedMaterial = initial_checked_pressed_material;

	//Set the text scale.
	_TextScale = text_scale;

	//Set the horizontal/vertical alignment.
	_HorizontalAlignment = horizontal_alignment;
	_VerticalAlignment = vertical_alignment;

	//Set the font.
	_Font = font;

	//Add the image primitive.
	{
		ImageUserInterfacePrimitiveDescription description;

		description._Type = UserInterfacePrimitiveType::IMAGE;
		description._Minimum = _ButtonMinimum;
		description._Maximum = _ButtonMaximum;
		description._Opacity = 1.0f;
		description._Material = initial_unchecked_idle_material;

		_ImagePrimitive = static_cast<ImageUserInterfacePrimitive *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfacePrimitive(&description));
	}

	//Set the text.
	SetText(text);

	//Update the material.
	UpdateMaterial();
}

/*
*	Default destructor.
*/
UserInterfaceCheckbox::~UserInterfaceCheckbox() NOEXCEPT
{
	//Destroy the primitives.
	UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_ImagePrimitive);

	if (_TextPrimitive)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfacePrimitive(_TextPrimitive);
	}
}

/*
*	Sets whether or not this checkbox is checked.
*/
void UserInterfaceCheckbox::SetIsChecked(const bool value) NOEXCEPT
{
	_IsChecked = value;

	UpdateMaterial();
}

/*
*	Sets the current state.
*/
void UserInterfaceCheckbox::SetCurrentState(const UserInterfaceButtonState value) NOEXCEPT
{
	_CurrentState = value;
	
	UpdateMaterial();
}

/*
*	Sets the text.
*/
void UserInterfaceCheckbox::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_TextPrimitive)
		{
			TextUserInterfacePrimitiveDescription description;

			description._Type = UserInterfacePrimitiveType::TEXT;
			description._Minimum = _TextMinimum;
			description._Maximum = _TextMaximum;
			description._Opacity = 1.0f;
			description._Font = _Font;
			description._Scale = _TextScale;
			description._HorizontalAlignment = _HorizontalAlignment;
			description._VerticalAlignment = _VerticalAlignment;
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
*	Returns the user interface primitives.
*/
void UserInterfaceCheckbox::RetrieveUserInterfacePrimitives(DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT output) const NOEXCEPT
{
	output->Emplace(_ImagePrimitive);

	if (_TextPrimitive)
	{
		output->Emplace(_TextPrimitive);
	}
}

/*
*	Updates the material.
*/
void UserInterfaceCheckbox::UpdateMaterial() NOEXCEPT
{
	switch (_CurrentState)
	{
		case UserInterfaceButtonState::IDLE:
		{
			_ImagePrimitive->_Material = _IsChecked ? _CheckedIdleMaterial : _UncheckedIdleMaterial;

			break;
		}

		case UserInterfaceButtonState::HOVERED:
		{
			_ImagePrimitive->_Material = _IsChecked ? _CheckedHoveredMaterial : _UncheckedHoveredMaterial;

			break;
		}

		case UserInterfaceButtonState::PRESSED:
		{
			_ImagePrimitive->_Material = _IsChecked ? _CheckedPressedMaterial : _UncheckedPressedMaterial;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}