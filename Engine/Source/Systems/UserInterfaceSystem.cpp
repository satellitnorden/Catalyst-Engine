//Header file.
#include <Systems/UserInterfaceSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>

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
*	Terminates the user interface system.
*/
void UserInterfaceSystem::Terminate() NOEXCEPT
{
	//Deallocate all user interface elements.
	for (UserInterfaceElement *const RESTRICT element : _UserInterfaceElements)
	{
		Memory::Free(element);
	}
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
			ButtonUserInterfaceElement *const RESTRICT element{ new (Memory::Allocate(sizeof(ButtonUserInterfaceElement))) ButtonUserInterfaceElement() };
			const ButtonUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const ButtonUserInterfaceElementDescription* const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::BUTTON;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
			element->_CurrentState = ButtonUserInterfaceElement::State::IDLE;
			element->_StartHoveredCallback = type_description->_StartHoveredCallback;
			element->_StopHoveredCallback = type_description->_StopHoveredCallback;
			element->_StartPressedCallback = type_description->_StartPressedCallback;
			element->_StopPressedCallback = type_description->_StopPressedCallback;
			element->_IdleTextureIndex = type_description->_IdleTextureIndex;
			element->_HoveredTextureIndex = type_description->_HoveredTextureIndex;
			element->_PressedTextureIndex = type_description->_PressedTextureIndex;

			_UserInterfaceElements.Emplace(element);

			return element;
		}

		case UserInterfaceElementType::IMAGE:
		{
			ImageUserInterfaceElement *const RESTRICT element{ new (Memory::Allocate(sizeof(ImageUserInterfaceElement))) ImageUserInterfaceElement() };
			const ImageUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const ImageUserInterfaceElementDescription *const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::IMAGE;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
			element->_TextureIndex = type_description->_ImageTextureIndex;

			_UserInterfaceElements.Emplace(element);

			return element;
		}

		case UserInterfaceElementType::TEXT:
		{
			TextUserInterfaceElement *const RESTRICT element{ new (Memory::Allocate(sizeof(TextUserInterfaceElement))) TextUserInterfaceElement() };
			const TextUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const TextUserInterfaceElementDescription *const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::TEXT;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
			element->_Font = type_description->_Font;
			element->_Scale = type_description->_Scale;
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
	_UserInterfaceElements.Erase(element);
}

/*
*	Updates the user interface system during the user interface update phase.
*/
void UserInterfaceSystem::UserInterfaceUpdate() NOEXCEPT
{
	//Cache the mouse data.
	const Vector2<float32> mouse_position{ InputSystem::Instance->GetMouseState()->_CurrentX, InputSystem::Instance->GetMouseState()->_CurrentY };
	const bool mouse_pressed{ InputSystem::Instance->GetMouseState()->_Left == ButtonState::Pressed || InputSystem::Instance->GetMouseState()->_Left == ButtonState::PressedHold };

	//Update the state of all button user interface elements and call the callbacks if necessary.
	for (UserInterfaceElement* const RESTRICT element : _UserInterfaceElements)
	{
		if (element->_Type == UserInterfaceElementType::BUTTON)
		{
			//Cache the type element.
			ButtonUserInterfaceElement *const RESTRICT type_element{ static_cast<ButtonUserInterfaceElement *const RESTRICT>(element) };

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
		}
	}
}