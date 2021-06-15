#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>


//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//User interface.
#include <UserInterface/UserInterfaceScene.h>

//World.
#include <World/Core/WorldPosition.h>

class UserInterfaceInitializationData final : public EntityInitializationData
{

public:

	//The user interface scene.
	UserInterfaceScene *RESTRICT _UserInterfaceScene;

	//The initial world position.
	WorldPosition _InitialWorldPosition;

	//The initial rotation.
	EulerAngles _InitialRotation;

	//The initial scale.
	Vector2<float32> _InitialScale;

};