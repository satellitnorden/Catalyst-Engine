//Header file.
#include <Systems/UserInterfaceSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//User interface.
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
			case UserInterfaceElementType::Image:
			{
				Memory::GlobalPoolDeAllocate<sizeof(ImageUserInterfaceElement)>(element);

				break;
			}

			case UserInterfaceElementType::Text:
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
		case UserInterfaceElementType::Image:
		{
			ImageUserInterfaceElement *const RESTRICT element{ new (Memory::GlobalPoolAllocate<sizeof(ImageUserInterfaceElement)>()) ImageUserInterfaceElement() };
			const ImageUserInterfaceElementDescription *const RESTRICT typeDescription{ static_cast<const ImageUserInterfaceElementDescription *const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::Image;
			element->_Minimum = typeDescription->_Minimum;
			element->_Maximum = typeDescription->_Maximum;
			element->_TextureIndex = typeDescription->_ImageTextureIndex;

			_UserInterfaceElements.EmplaceSlow(element);

			return element;
		}

		case UserInterfaceElementType::Text:
		{
			TextUserInterfaceElement *const RESTRICT element{ new (Memory::GlobalPoolAllocate<sizeof(TextUserInterfaceElement)>()) TextUserInterfaceElement() };
			const TextUserInterfaceElementDescription *const RESTRICT typeDescription{ static_cast<const TextUserInterfaceElementDescription *const RESTRICT>(description) };

			element->_Type = UserInterfaceElementType::Text;
			element->_Minimum = typeDescription->_Minimum;
			element->_Maximum = typeDescription->_Maximum;
			element->_Font = typeDescription->_Font;
			element->_Text = std::move(typeDescription->_Text);

			_UserInterfaceElements.EmplaceSlow(element);

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