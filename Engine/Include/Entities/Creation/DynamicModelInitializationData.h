#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Physics.
#include <Physics/ModelCollisionData.h>
#include <Physics/ModelPhysicsSimulationData.h>
#include <Physics/PhysicsCore.h>

//Rendering.
#include <Rendering/Native/MaterialIndexCollection.h>

//Resources.
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldTransform.h>

class DynamicModelInitializationData final : public EntityInitializationData
{

public:

	//The initial world transform.
	WorldTransform _InitialWorldTransform;

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The material index collection.
	MaterialIndexCollection _MaterialIndexCollection;

	//The model collision data.
	ModelCollisionData _ModelCollisionData;

	//Denotes whether or not to simulate physics.
	bool _SimulatePhysics;

	//The model physics simulation data.
	ModelPhysicsSimulationData _ModelPhysicsSimulationData;

};