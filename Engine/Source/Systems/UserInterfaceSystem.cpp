//Header file.
#include <Systems/UserInterfaceSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

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
*	Terminates the user interface system.
*/
void UserInterfaceSystem::Terminate() NOEXCEPT
{
	//Deallocate all user interface elements.
	for (UserInterfaceElement *const RESTRICT element : _UserInterfaceElements)
	{
		switch (element->_Type)
		{
			case UserInterfaceElementType::BUTTON:
			{
				Memory::GlobalPoolDeAllocate<sizeof(ButtonUserInterfaceElement)>(element);

				break;
			}

			case UserInterfaceElementType::IMAGE:
			{
				Memory::GlobalPoolDeAllocate<sizeof(ImageUserInterfaceElement)>(element);

				break;
			}

			case UserInterfaceElementType::TEXT:
			{
				Memory::GlobalPoolDeAllocate<sizeof(TextUserInterfaceElement)>(element);

				break;
			}

			default:
			{
				ASSERT(false, "Unhandled case!");

				break;
			}
		}
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
			ButtonUserInterfaceElement *const RESTRICT element{ new (Memory::GlobalPoolAllocate<sizeof(ButtonUserInterfaceElement)>()) ButtonUserInterfaceElement() };
			const ButtonUserInterfaceElementDescription *const RESTRICT type_description{ static_cast<const ButtonUserInterfaceElementDescription* const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::IMAGE;
			element->_Minimum = type_description->_Minimum;
			element->_Maximum = type_description->_Maximum;
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
			ImageUserInterfaceElement *const RESTRICT element{ new (Memory::GlobalPoolAllocate<sizeof(ImageUserInterfaceElement)>()) ImageUserInterfaceElement() };
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
			TextUserInterfaceElement *const RESTRICT element{ new (Memory::GlobalPoolAllocate<sizeof(TextUserInterfaceElement)>()) TextUserInterfaceElement() };
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