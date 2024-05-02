#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Generated.
#include <Generated/UserInterface.Generated.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

/*
*	Adds this in the header file of your user interface scene to automatically generate a factory. (:
*/
#define CATALYST_USER_INTERFACE_SCENE(X)

namespace UserInterfaceSceneFactory
{

	/*
	*	Creates a user interface scene with the given identifier.
	*/
	NO_DISCARD UserInterfaceScene *const RESTRICT Create(const UserInterfaceSceneIdentifier identifier) NOEXCEPT;

	/*
	*	Destroys the given user interface scene.
	*/
	void Destroy(const UserInterfaceSceneIdentifier identifier, UserInterfaceScene *const RESTRICT user_interface_scene) NOEXCEPT;

}