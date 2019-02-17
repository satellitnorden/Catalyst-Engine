#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/PhysicsComponent.h>
#include <Components/TransformComponent.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

class PhysicsInitializationData final : public EntityInitializationData
{

public:

	//The physics component.
	PhysicsComponent _PhysicsComponent;

	//The transform component.
	TransformComponent _TransformComponent;

};