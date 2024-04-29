#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//Systems.
#include <Systems/System.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>
#include <UserInterface/UserInterfacePrimitiveDescription.h>
#include <UserInterface/UserInterfaceScene.h>
#include <UserInterface/UserInterfaceSceneFactory.h>

class UserInterfaceSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		UserInterfaceSystem
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UserInterfaceSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the user interface system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the user interface system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Creates a user interface primitive.
	*/
	RESTRICTED NO_DISCARD UserInterfacePrimitive *const RESTRICT CreateUserInterfacePrimitive(const UserInterfacePrimitiveDescription *const RESTRICT description, const bool is_three_dimensional) NOEXCEPT;

	/*
	*	Destroys a user interface primitive.
	*/
	void DestroyUserInterfacePrimitive(UserInterfacePrimitive *const RESTRICT primitive) NOEXCEPT;

	/*
	*	Returns the user interface primitives.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const DynamicArray<UserInterfacePrimitive *RESTRICT> *const RESTRICT GetUserInterfacePrimitives() const NOEXCEPT
	{
		return &_UserInterfacePrimitives;
	}

	/*
	*	Registers a user interface scene factory.
	*/
	FORCE_INLINE void RegisterUserInterfaceSceneFactory(const UserInterfaceSceneFactory &value) NOEXCEPT
	{
		_RegisteredUserInterfaceSceneFactories.Emplace(value);
	}

	/*
	*	Returns all the registered user interface scene factories.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<UserInterfaceSceneFactory> &GetRegisteredUserInterfaceSceneFactories() const NOEXCEPT
	{
		return _RegisteredUserInterfaceSceneFactories;
	}

	/*
	*	Creates a user interface scene with the given identifier.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD UserInterfaceScene *const RESTRICT CreateUserInterfaceScene(const HashString identifier) NOEXCEPT
	{
		UserInterfaceScene *RESTRICT new_user_interface_scene{ nullptr };

		for (const UserInterfaceSceneFactory &factory : _RegisteredUserInterfaceSceneFactories)
		{
			if (factory.GetIdentifier() == identifier)
			{
				new_user_interface_scene = factory.Create();
				new_user_interface_scene->SetName(factory.GetName());
				new_user_interface_scene->SetIdentifier(factory.GetIdentifier());

				break;
			}
		}

		return new_user_interface_scene;
	}

	/*
	*	Activates the given scene.
	*/
	FORCE_INLINE void ActivateScene(UserInterfaceScene *const RESTRICT scene) NOEXCEPT
	{
		_ActivationQueue.Push(scene);
	}

	/*
	*	Deactivates the given scene.
	*/
	FORCE_INLINE void DeactivateScene(UserInterfaceScene *const RESTRICT scene) NOEXCEPT
	{
		_DeactivationQueue.Push(scene);
	}

	/*
	*	Destroys a user interface scene.
	*/
	FORCE_INLINE void DestroyUserInterfaceScene(UserInterfaceScene *const RESTRICT scene) NOEXCEPT
	{
		_DestructionQueue.Push(scene);
	}

	/*
	*	Deactivates all active scenes.
	*/
	FORCE_INLINE void DeactivateAllActiveScenes() NOEXCEPT
	{
		for (UserInterfaceScene *const RESTRICT scene : _ActiveUserInterfaceScenes)
		{
			DeactivateScene(scene);
		}
	}

private:

	//The registered user interface scene factories.
	DynamicArray<UserInterfaceSceneFactory> _RegisteredUserInterfaceSceneFactories;

	//The active user interface scenes.
	DynamicArray<UserInterfaceScene *RESTRICT> _ActiveUserInterfaceScenes;

	//The activation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 8, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _ActivationQueue;

	//The deactivation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 8, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DeactivationQueue;

	//The destruction queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 8, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DestructionQueue;

	//Container for all user interface primitives.
	DynamicArray<UserInterfacePrimitive *RESTRICT> _UserInterfacePrimitives;

	/*
	*	Updates the user interface system during the user interface update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

};