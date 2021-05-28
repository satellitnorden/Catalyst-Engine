#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//User interface.
#include <UserInterface/UserInterfacePrimitive.h>
#include <UserInterface/UserInterfacePrimitiveDescription.h>
#include <UserInterface/UserInterfaceScene.h>

class UserInterfaceSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(UserInterfaceSystem);

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
	*	Creates a user interface primitive.
	*/
	RESTRICTED NO_DISCARD UserInterfacePrimitive *const RESTRICT CreateUserInterfacePrimitive(const UserInterfacePrimitiveDescription *const RESTRICT description) NOEXCEPT;

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
	*	Deactivates all active scenes.
	*/
	FORCE_INLINE void DeactivateAllActiveScenes() NOEXCEPT
	{
		for (UserInterfaceScene *const RESTRICT scene : _CurrentUserInterfaceScenes)
		{
			DeactivateScene(scene);
		}
	}

private:

	//The current Clairvoyant user interface scenes.
	DynamicArray<UserInterfaceScene *RESTRICT> _CurrentUserInterfaceScenes;

	//The activation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 4, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _ActivationQueue;

	//The deactivation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 4, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DeactivationQueue;

	//Container for all user interface primitives.
	DynamicArray<UserInterfacePrimitive *RESTRICT> _UserInterfacePrimitives;

	/*
	*	Updates the user interface system during the user interface update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

};