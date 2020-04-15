//Header file.
#include <Systems/UserInterfaceSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/MemorySystem.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElement.h>
#include <UserInterface/ButtonUserInterfaceElementDescription.h>
#include <UserInterface/ImageUserInterfaceElement.h>
#include <UserInterface/ImageUserInterfaceElementDescription.h>
#include <UserInterface/TextUserInterfaceElement.h>
#include <UserInterface/TextUserInterfaceElementDescription.h>

//Singleton definition.
DEFINE_SINGLETON(UserInterfaceSystem);

/*
*	Initializes the user interface system.
*/
void UserInterfaceSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<UserInterfaceSystem *const RESTRICT>(arguments)->UserInterfaceUpdate();
	},
	this,
	UpdatePhase::USER_INTERFACE,
	UpdatePhase::LOGIC);
}

/*
*	Creates a user interface element.
*/
RESTRICTED NO_DISCARD UserInterfaceElement *const RESTRICT UserInterfaceSystem::CreateUserInterfaceElement(const UserInterfaceElementDescription *const RESTRICT description) NOEXCEPT
{
	switch (description->_Type)
	{
		case UserInterfaceElementType::BUTTON:
		{
			ButtonUserInterfaceElement *const RESTRICT element{ new (MemorySystem::Instance->TypeAllocate<ButtonUserInterfaceElement>()) ButtonUserInterfaceElement() };
			const ButtonUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const ButtonUserInterfaceElementDescription* const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::BUTTON;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
			element->_CurrentState = ButtonUserInterfaceElement::State::IDLE;
			element->_StartHoveredCallback = type_description->_StartHoveredCallback;
			element->_StopHoveredCallback = type_description->_StopHoveredCallback;
			element->_StartPressedCallback = type_description->_StartPressedCallback;
			element->_StopPressedCallback = type_description->_StopPressedCallback;
			element->_IdleMaterial = type_description->_IdleMaterial;
			element->_HoveredMaterial = type_description->_HoveredMaterial;
			element->_PressedMaterial = type_description->_PressedMaterial;

			_UserInterfaceElements.Emplace(element);

			return element;
		}

		case UserInterfaceElementType::IMAGE:
		{
			ImageUserInterfaceElement *const RESTRICT element{ new (MemorySystem::Instance->TypeAllocate<ImageUserInterfaceElement>()) ImageUserInterfaceElement() };
			const ImageUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const ImageUserInterfaceElementDescription *const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::IMAGE;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
			element->_Material = type_description->_Material;

			_UserInterfaceElements.Emplace(element);

			return element;
		}

		case UserInterfaceElementType::TEXT:
		{
			TextUserInterfaceElement *const RESTRICT element{ new (MemorySystem::Instance->TypeAllocate<TextUserInterfaceElement>()) TextUserInterfaceElement() };
			const TextUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const TextUserInterfaceElementDescription *const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::TEXT;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
			element->_Font = type_description->_Font;
			element->_Scale = type_description->_Scale;
			element->_HorizontalAlignment = type_description->_HorizontalAlignment;
			element->_VerticalAlignment = type_description->_VerticalAlignment;
			element->_Text = std::move(type_description->_Text);

			_UserInterfaceElements.Emplace(element);

			return element;
		}

		default:
		{
			ASSERT(false, "Unhandled case!");

			return nullptr;
		}
	}
}

/*
*	Destroys a user interface element.
*/
void UserInterfaceSystem::DestroyUserInterfaceElement(UserInterfaceElement *const RESTRICT element) NOEXCEPT
{
	switch (element->_Type)
	{
		case UserInterfaceElementType::BUTTON:
		{
			MemorySystem::Instance->TypeFree<ButtonUserInterfaceElement>(static_cast<ButtonUserInterfaceElement *const RESTRICT>(element));

			break;
		}

		case UserInterfaceElementType::IMAGE:
		{
			MemorySystem::Instance->TypeFree<ImageUserInterfaceElement>(static_cast<ImageUserInterfaceElement *const RESTRICT>(element));

			break;
		}

		case UserInterfaceElementType::TEXT:
		{
			MemorySystem::Instance->TypeFree<TextUserInterfaceElement>(static_cast<TextUserInterfaceElement *const RESTRICT>(element));

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	_UserInterfaceElements.Erase(element);
}

/*
*	Updates the user interface system during the user interface update phase.
*/
void UserInterfaceSystem::UserInterfaceUpdate() NOEXCEPT
{
	//Cache the input data.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState() };
	const Vector2<float32> mouse_position{ InputSystem::Instance->GetMouseState()->_CurrentX, InputSystem::Instance->GetMouseState()->_CurrentY };
	const bool mouse_pressed{ InputSystem::Instance->GetMouseState()->_Left == ButtonState::Pressed || InputSystem::Instance->GetMouseState()->_Left == ButtonState::PressedHold };

	//Update which button is gamepad selected.
	if (InputSystem::Instance->GetLastUpdatedInputDeviceType() == InputDeviceType::GAMEPAD)
	{
		ChooseDefaultGamepadSelectedButton();

		if (gamepad_state->_DpadUp == ButtonState::Pressed)
		{
			ChooseNewGamepadSelectedButton(Vector2<float32>(0.0f, 1.0f));
		}

		else if (gamepad_state->_DpadDown == ButtonState::Pressed)
		{
			ChooseNewGamepadSelectedButton(Vector2<float32>(0.0f, -1.0f));
		}

		else if (gamepad_state->_DpadLeft == ButtonState::Pressed)
		{
			ChooseNewGamepadSelectedButton(Vector2<float32>(-1.0f, 0.0f));
		}

		else if (gamepad_state->_DpadRight == ButtonState::Pressed)
		{
			ChooseNewGamepadSelectedButton(Vector2<float32>(1.0f, 0.0f));
		}
	}

	//Update the state of all button user interface elements and call the callbacks if necessary.
	for (uint64 i{ 0 }; i < _UserInterfaceElements.Size(); ++i)
	{
		//Cache the element.
		UserInterfaceElement *const RESTRICT element{ _UserInterfaceElements[i] };

		if (element->_Type == UserInterfaceElementType::BUTTON)
		{
			//Cache the type element.
			ButtonUserInterfaceElement *const RESTRICT type_element{ static_cast<ButtonUserInterfaceElement *const RESTRICT>(element) };

			//Apply different logic based on which input device type was updated last.
			switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
			{
				case InputDeviceType::GAMEPAD:
				{
					//Mutate the state and call callbacks.
					switch (type_element->_CurrentState)
					{
						case ButtonUserInterfaceElement::State::IDLE:
						{
							if (type_element->_IsGamepadSelected)
							{
								if (gamepad_state->_A == ButtonState::Pressed)
								{
									if (type_element->_StartPressedCallback)
									{
										type_element->_StartPressedCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::PRESSED;
								}

								else
								{
									type_element->_CurrentState = ButtonUserInterfaceElement::State::HOVERED;
								}
							}

							break;
						}

						case ButtonUserInterfaceElement::State::HOVERED:
						{
							if (type_element->_IsGamepadSelected)
							{
								if (gamepad_state->_A == ButtonState::Pressed)
								{
									if (type_element->_StartPressedCallback)
									{
										type_element->_StartPressedCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::PRESSED;
								}
							}

							else
							{
								type_element->_CurrentState = ButtonUserInterfaceElement::State::IDLE;
							}

							break;
						}

						case ButtonUserInterfaceElement::State::PRESSED:
						{
							if (type_element->_IsGamepadSelected)
							{
								if (gamepad_state->_A == ButtonState::Released)
								{
									if (type_element->_StopPressedCallback)
									{
										type_element->_StopPressedCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::HOVERED;
								}
							}

							else
							{
								type_element->_CurrentState = ButtonUserInterfaceElement::State::IDLE;
							}

							break;
						}
					}

					break;
				}

				case InputDeviceType::KEYBOARD:
				case InputDeviceType::MOUSE:
				{
					//Determine if the mouse is inside the element's bounding box.
					const bool is_inside{	mouse_position._X >= type_element->_Minimum._X
											&& mouse_position._X <= type_element->_Maximum._X
											&& mouse_position._Y >= type_element->_Minimum._Y
											&& mouse_position._Y <= type_element->_Maximum._Y };

					//Mutate the state and call callbacks.
					switch (type_element->_CurrentState)
					{
						case ButtonUserInterfaceElement::State::IDLE:
						{
							if (is_inside)
							{
								if (mouse_pressed)
								{
									if (type_element->_StartPressedCallback)
									{
										type_element->_StartPressedCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::PRESSED;
								}

								else
								{
									if (type_element->_StartHoveredCallback)
									{
										type_element->_StartHoveredCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::HOVERED;
								}
							}

							break;
						}

						case ButtonUserInterfaceElement::State::HOVERED:
						{
							if (is_inside)
							{
								if (mouse_pressed)
								{
									if (type_element->_StartPressedCallback)
									{
										type_element->_StartPressedCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::PRESSED;
								}
							}

							else
							{
								if (type_element->_StopHoveredCallback)
								{
									type_element->_StopHoveredCallback(type_element);
								}

								type_element->_CurrentState = ButtonUserInterfaceElement::State::IDLE;
							}

							break;
						}

						case ButtonUserInterfaceElement::State::PRESSED:
						{
							if (is_inside)
							{
								if (!mouse_pressed)
								{
									if (type_element->_StopPressedCallback)
									{
										type_element->_StopPressedCallback(type_element);
									}

									type_element->_CurrentState = ButtonUserInterfaceElement::State::HOVERED;
								}
							}

							else
							{
								if (type_element->_StopPressedCallback)
								{
									type_element->_StopPressedCallback(type_element);
								}

								type_element->_CurrentState = ButtonUserInterfaceElement::State::IDLE;
							}

							break;
						}
					}

					break;
				}
			}
		}
	}
}

/*
*	Chooses a default gamepad selected button.
*/
void UserInterfaceSystem::ChooseDefaultGamepadSelectedButton() NOEXCEPT
{
	//First of all, find the first and the currently gamepad selected button.
	ButtonUserInterfaceElement *RESTRICT first_button{ nullptr };
	ButtonUserInterfaceElement *RESTRICT currently_gamepad_selected_button{ nullptr };

	//Update the state of all button user interface elements and call the callbacks if necessary.
	for (uint64 i{ 0 }; i < _UserInterfaceElements.Size(); ++i)
	{
		//Cache the element.
		UserInterfaceElement *const RESTRICT element{ _UserInterfaceElements[i] };

		//Is this element a button?
		if (element->_Type == UserInterfaceElementType::BUTTON)
		{
			//Cache the type element.
			ButtonUserInterfaceElement* const RESTRICT type_element{ static_cast<ButtonUserInterfaceElement* const RESTRICT>(element) };

			//Remember the first button.
			first_button = first_button ? first_button : type_element;

			//Remember the currently gamepad selected button.
			if (type_element->_IsGamepadSelected)
			{
				currently_gamepad_selected_button = type_element;

				break;
			}
		}
	}

	//If there's no currently gamepad selected button, pick the first one found.
	if (!currently_gamepad_selected_button && first_button)
	{
		first_button->_IsGamepadSelected = true;
	}
}

/*
*	Chooses a new gamepad selected button in the given direction.
*/
void UserInterfaceSystem::ChooseNewGamepadSelectedButton(const Vector2<float32>& direction) NOEXCEPT
{
	//First of all, find the currently gamepad selected button.
	ButtonUserInterfaceElement *RESTRICT currently_gamepad_selected_button{ nullptr };

	//Update the state of all button user interface elements and call the callbacks if necessary.
	for (uint64 i{ 0 }; i < _UserInterfaceElements.Size(); ++i)
	{
		//Cache the element.
		UserInterfaceElement *const RESTRICT element{ _UserInterfaceElements[i] };

		//Is this element a button?
		if (element->_Type == UserInterfaceElementType::BUTTON)
		{
			//Cache the type element.
			ButtonUserInterfaceElement* const RESTRICT type_element{ static_cast<ButtonUserInterfaceElement* const RESTRICT>(element) };

			//Remember the currently gamepad selected button.
			if (type_element->_IsGamepadSelected)
			{
				currently_gamepad_selected_button = type_element;

				break;
			}
		}
	}

	//Calculate the center of the currently gamepad selected button.
	const Vector2<float32> currently_gamepad_selected_button_center{ currently_gamepad_selected_button->_Minimum + ((currently_gamepad_selected_button->_Maximum - currently_gamepad_selected_button->_Minimum) * 0.5f) };

	//Find another button that is close and in the general direction.
	ButtonUserInterfaceElement *RESTRICT new_gamepad_selected_button{ nullptr };
	float32 shortest_distance{ FLOAT_MAXIMUM };

	for (uint64 i{ 0 }; i < _UserInterfaceElements.Size(); ++i)
	{
		//Cache the element.
		UserInterfaceElement *const RESTRICT element{ _UserInterfaceElements[i] };

		//Is this element a button, and not the currently selected gamepad button?
		if (element->_Type == UserInterfaceElementType::BUTTON && element != currently_gamepad_selected_button)
		{
			//Cache the type element.
			ButtonUserInterfaceElement* const RESTRICT type_element{ static_cast<ButtonUserInterfaceElement* const RESTRICT>(element) };

			//Calculate the center of this button.
			const Vector2<float32> button_center{ type_element->_Minimum + ((type_element->_Maximum - type_element->_Minimum) * 0.5f) };

			//Calculate the distance to ths button.
			const float32 distance_to_button{ Vector2<float32>::Length(button_center - currently_gamepad_selected_button_center) };

			//Calculate the direction to this button.
			const Vector2<float32> direction_to_button{ (button_center - currently_gamepad_selected_button_center) / distance_to_button };

			//Test the angle.
			if (Vector2<float32>::DotProduct(direction, direction_to_button) >= 0.5f)
			{
				//Test the distance.
				if (shortest_distance > distance_to_button)
				{
					new_gamepad_selected_button = type_element;
					shortest_distance = distance_to_button;
				}

			}
		}
	}

	//Is there a new gamepad selected button?
	if (new_gamepad_selected_button)
	{
		currently_gamepad_selected_button->_IsGamepadSelected = false;
		new_gamepad_selected_button->_IsGamepadSelected = true;
	}
}