//Header file.
#include <Systems/UserInterfaceSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//User interface.
#include <UserInterface/ImageUserInterfaceElement.h>
#include <UserInterface/ImageUserInterfaceElementDescription.h>

/*
*	Updates the user interface system.
*/
void UserInterfaceSystem::Update(const UpdateContext *const RESTRICT context) NOEXCEPT
{

}

/*
*	Terminates the user interface system.
*/
void UserInterfaceSystem::Terminate() NOEXCEPT
{
	//Deallocate all user interface elements.
	for (UserInterfaceElement *const RESTRICT element : ComponentManager::WriteSingletonComponent<UserInterfaceComponent>()->_UserInterfaceElements)
	{
		Memory::GlobalPoolDeAllocate<sizeof(ImageUserInterfaceElement)>(element);
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
			element->_ActiveTextureIndex = typeDescription->_ImageTextureIndex;

			ComponentManager::WriteSingletonComponent<UserInterfaceComponent>()->_UserInterfaceElements.EmplaceSlow(element);

			return element;
		}

		default:
		{
			ASSERT(false, "Unhandled case!");

			return nullptr;
		}
	}
}