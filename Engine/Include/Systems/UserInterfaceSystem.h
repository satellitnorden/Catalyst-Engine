#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Concurrency.
#include <Concurrency/AtomicQueue.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>
#include <UserInterface/UserInterfaceElementDescription.h>
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
	*	Creates a user interface element.
	*/
	RESTRICTED NO_DISCARD UserInterfaceElement *const RESTRICT CreateUserInterfaceElement(const UserInterfaceElementDescription *const RESTRICT description) NOEXCEPT;

	/*
	*	Destroys a user interface element.
	*/
	void DestroyUserInterfaceElement(UserInterfaceElement *const RESTRICT element) NOEXCEPT;

	/*
	*	Returns the user interface elements.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const DynamicArray<UserInterfaceElement *RESTRICT> *const RESTRICT GetUserInterfaceElements() const NOEXCEPT
	{
		return &_UserInterfaceElements;
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

private:

	//The current Clairvoyant user interface scenes.
	DynamicArray<UserInterfaceScene *RESTRICT> _CurrentUserInterfaceScenes;

	//The activation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 4, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _ActivationQueue;

	//The deactivation queue.
	AtomicQueue<UserInterfaceScene *RESTRICT, 4, AtomicQueueMode::MULTIPLE, AtomicQueueMode::SINGLE> _DeactivationQueue;

	//Container for all user interface elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

	/*
	*	Updates the user interface system during the user interface update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

};