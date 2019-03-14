#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Transient/TransformComponent.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

class CollisionInitializationData final : public EntityInitializationData
{

public:

	//The physics shape.
	PhysicsShape _PhysicsShape;

	//The transform component.
	TransformComponent _TransformComponent;

};