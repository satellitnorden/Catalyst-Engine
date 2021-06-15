#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//World.
#include <World/Core/WorldPosition.h>

class UserInterfaceComponent final
{

public:

	//The user interface scene.
	UserInterfaceScene *RESTRICT _UserInterfaceScene;

	//The world position.
	WorldPosition _WorldPosition;

	//The rotation.
	EulerAngles _Rotation;

	//The scale.
	Vector2<float32> _Scale;

};