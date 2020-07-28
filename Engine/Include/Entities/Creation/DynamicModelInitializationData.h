#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/EntityInitializationData.h>

//Physics.
#include <Physics/ModelCollisionConfiguration.h>
#include <Physics/ModelPhysicsSimulationData.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
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

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The model collision configuration.
	ModelCollisionConfiguration _ModelCollisionConfiguration;

	//Denotes whether or not to simulate physics.
	bool _SimulatePhysics;

	//The model physics simulation data.
	ModelPhysicsSimulationData _ModelPhysicsSimulationData;

};