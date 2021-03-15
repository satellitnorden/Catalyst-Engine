//Header file.
#include <UserInterface/UserInterfaceButton.h>

//Systems.
#include <Systems/ResourceSystem.h>
#include <Systems/UserInterfaceSystem.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElementDescription.h>
#include <UserInterface/TextUserInterfaceElementDescription.h>

/*
*	Default constructor.
*/
UserInterfaceButton::UserInterfaceButton(	const Vector2<float32> initial_minimum,
											const Vector2<float32> initial_maximum,
											const Callback initial_start_hovered_callback,
											const Callback initial_stop_hovered_callback,
											const Callback initial_start_pressed_callback,
											const Callback initial_stop_pressed_callback,
											const UserInterfaceMaterial &initial_idle_material,
											const UserInterfaceMaterial &initial_hovered_material,
											const UserInterfaceMaterial &initial_pressed_material,
											const char *const RESTRICT text) NOEXCEPT
{
	//Set the minimum/maximum.
	_Minimum = initial_minimum;
	_Maximum = initial_maximum;

	//Set the current state.
	_CurrentState = State::IDLE;
	
	//Set the callbacks.
	_StartHoveredCallback = initial_start_hovered_callback;
	_StopHoveredCallback = initial_stop_hovered_callback;
	_StartPressedCallback = initial_start_pressed_callback;
	_StopPressedCallback = initial_stop_pressed_callback;

	//Set the materials.
	_IdleMaterial = initial_idle_material;
	_HoveredMaterial = initial_hovered_material;
	_PressedMaterial = initial_pressed_material;

	//Add the image element.
	{
		ImageUserInterfaceElementDescription description;

		description._Type = UserInterfaceElementType::IMAGE;
		description._Minimum = initial_minimum;
		description._Maximum = initial_maximum;
		description._Opacity = 1.0f;
		description._Material = initial_idle_material;

		_ImageElement = static_cast<ImageUserInterfaceElement *RESTRICT>(UserInterfaceSystem::Instance->CreateUserInterfaceElement(&description));
	}

	//Set the text.
	SetText(text);
}

/*
*	Default destructor.
*/
UserInterfaceButton::~UserInterfaceButton() NOEXCEPT
{
	//Destroy the elements.
	UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_ImageElement);

	if (_TextElement)
	{
		UserInterfaceSystem::Instance->DestroyUserInterfaceElement(_TextElement);
	}
}

/*
*	Sets the current state.
*/
void UserInterfaceButton::SetCurrentState(const State value) NOEXCEPT
{
	_CurrentState = value;

	switch (_CurrentState)
	{
		case State::IDLE:
		{
			_ImageElement->_Material = _IdleMaterial;

			break;
		}

		case State::HOVERED:
		{
			_ImageElement->_Material = _HoveredMaterial;

			break;
		}

		case State::PRESSED:
		{
			_ImageElement->_Material = _PressedMaterial;

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
*	Sets the text.
*/
void UserInterfaceButton::SetText(const char *const RESTRICT text) NOEXCEPT
{
	if (text && text != "")
	{
		if (!_TextElement)
		{
			TextUserInterfaceElementDescription description;

			description._Type = UserInterfaceElementType::TEXT;
			description._Minimum = _Minimum;
			description._Maximum = _Maximum;
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