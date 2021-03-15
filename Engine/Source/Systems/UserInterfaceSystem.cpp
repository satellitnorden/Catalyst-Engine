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
	UpdatePhase::LOGIC,
	false);
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
			element->_Opacity = type_description->_Opacity;
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
			element->_Opacity = type_description->_Opacity;
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
			element->_Opacity = type_description->_Opacity;
			element->_FontResource = type_description->_FontResource;
			element->_Scale = type_description->_Scale;
			element->_HorizontalAlignment = type_description->_HorizontalAlignment;
			element->_VerticalAlignment = type_description->_VerticalAlignment;
			element->_TextSmoothingFactor = type_description->_TextSmoothingFactor;
			element->_Opacity = type_description->_Opacity;
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

	_UserInterfaceElements.Erase<false>(element);
}

/*
*	Updates the user interface system during the user interface update phase.
*/
void UserInterfaceSystem::UserInterfaceUpdate() NOEXCEPT
{
	//Process the deactivation queue.
	while (UserInterfaceScene *const RESTRICT *const RESTRICT scene{ _DeactivationQueue.Pop() })
	{
		if ((*scene)->GetIsActive())
		{
			(*scene)->OnDeactivated();

			_CurrentUserInterfaceScenes.Erase<false>(*scene);

			(*scene)->SetIsActive(false);
		}
	}

	//Process the activation queue.
	while (UserInterfaceScene *const RESTRICT *const RESTRICT scene{ _ActivationQueue.Pop() })
	{
		if (!(*scene)->GetIsActive())
		{
			_CurrentUserInterfaceScenes.Emplace(*scene);

			(*scene)->OnActivated();

			(*scene)->SetIsActive(true);
		}
	}

	//Update all the current scenes.
	for (UserInterfaceScene *const RESTRICT scene : _CurrentUserInterfaceScenes)
	{
		scene->Update();
	}
}