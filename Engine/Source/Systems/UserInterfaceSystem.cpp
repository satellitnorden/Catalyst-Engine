//Header file.
#include <Systems/UserInterfaceSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/MemorySystem.h>

//User interface.
#include <UserInterface/ImageUserInterfacePrimitive.h>
#include <UserInterface/ImageUserInterfacePrimitiveDescription.h>
#include <UserInterface/TextUserInterfacePrimitive.h>
#include <UserInterface/TextUserInterfacePrimitiveDescription.h>

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
	false,
	false);
}

/*
*	Creates a user interface primitive.
*/
RESTRICTED NO_DISCARD UserInterfacePrimitive *const RESTRICT UserInterfaceSystem::CreateUserInterfacePrimitive(const UserInterfacePrimitiveDescription *const RESTRICT description) NOEXCEPT
{
	switch (description->_Type)
	{
		case UserInterfacePrimitiveType::IMAGE:
		{
			ImageUserInterfacePrimitive *const RESTRICT primitive{ new (MemorySystem::Instance->TypeAllocate<ImageUserInterfacePrimitive>()) ImageUserInterfacePrimitive() };
			const ImageUserInterfacePrimitiveDescription *const RESTRICT type_description{ static_cast<const ImageUserInterfacePrimitiveDescription *const RESTRICT>(description) };

			primitive->_Type = UserInterfacePrimitiveType::IMAGE;
			primitive->_Minimum = type_description->_Minimum;
			primitive->_Maximum = type_description->_Maximum;
			primitive->_Opacity = type_description->_Opacity;
			primitive->_Material = type_description->_Material;

			_UserInterfacePrimitives.Emplace(primitive);

			return primitive;
		}

		case UserInterfacePrimitiveType::TEXT:
		{
			TextUserInterfacePrimitive *const RESTRICT primitive{ new (MemorySystem::Instance->TypeAllocate<TextUserInterfacePrimitive>()) TextUserInterfacePrimitive() };
			const TextUserInterfacePrimitiveDescription *const RESTRICT type_description{ static_cast<const TextUserInterfacePrimitiveDescription *const RESTRICT>(description) };

			primitive->_Type = UserInterfacePrimitiveType::TEXT;
			primitive->_Minimum = type_description->_Minimum;
			primitive->_Maximum = type_description->_Maximum;
			primitive->_Opacity = type_description->_Opacity;
			primitive->_FontResource = type_description->_FontResource;
			primitive->_Scale = type_description->_Scale;
			primitive->_HorizontalAlignment = type_description->_HorizontalAlignment;
			primitive->_VerticalAlignment = type_description->_VerticalAlignment;
			primitive->_TextSmoothingFactor = type_description->_TextSmoothingFactor;
			primitive->_Opacity = type_description->_Opacity;
			primitive->_Text = std::move(type_description->_Text);

			_UserInterfacePrimitives.Emplace(primitive);

			return primitive;
		}

		default:
		{
			ASSERT(false, "Unhandled case!");

			return nullptr;
		}
	}
}

/*
*	Destroys a user interface primitive.
*/
void UserInterfaceSystem::DestroyUserInterfacePrimitive(UserInterfacePrimitive *const RESTRICT primitive) NOEXCEPT
{
	switch (primitive->_Type)
	{
		case UserInterfacePrimitiveType::IMAGE:
		{
			MemorySystem::Instance->TypeFree<ImageUserInterfacePrimitive>(static_cast<ImageUserInterfacePrimitive *const RESTRICT>(primitive));

			break;
		}

		case UserInterfacePrimitiveType::TEXT:
		{
			MemorySystem::Instance->TypeFree<TextUserInterfacePrimitive>(static_cast<TextUserInterfacePrimitive *const RESTRICT>(primitive));

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	_UserInterfacePrimitives.Erase<false>(primitive);
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