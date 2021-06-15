#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//World.
#include <World/Core/WorldTransform.h>

class UserInterfaceComponent final
{

public:

	//The user interface scene.
	UserInterfaceScene *RESTRICT _UserInterfaceScene;

	//The world transform.
	WorldTransform _WorldTransform;

};