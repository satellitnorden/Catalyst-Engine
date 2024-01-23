#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Core/Component.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>
#include <Physics/Native/ModelCollisionConfiguration.h>
#include <Physics/Native/ModelSimulationConfiguration.h>

//Resources.
#include <Resources/Core/MaterialResource.h>
#include <Resources/Core/ModelResource.h>
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class StaticModelSharedData final
{

};

class StaticModelInitializationData final : public ComponentInitializationData
{

public:

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The model collision configuration.
	ModelCollisionConfiguration _ModelCollisionConfiguration;

	//The model simulation configuration.
	ModelSimulationConfiguration _ModelSimulationConfiguration;

};

class StaticModelInstanceData final
{

public:

	//The model resource.
	ResourcePointer<ModelResource> _ModelResource;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The material resources.
	StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _MaterialResources;

	//The physics actor handle.
	ActorHandle _PhysicsActorHandle;

	//The model collision configuration.
	ModelCollisionConfiguration _ModelCollisionConfiguration;

	//The model simulation configuration.
	ModelSimulationConfiguration _ModelSimulationConfiguration;

	//The level of detail indices.
	StaticArray<uint8, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _LevelOfDetailIndices;

	//Bitmask for the visible meshes.
	uint8 _MeshesVisibleMask;

	//The visibility flags.
	VisibilityFlags _VisibilityFlags;

};

DECLARE_COMPONENT(StaticModelComponent, StaticModelSharedData, StaticModelInitializationData, StaticModelInstanceData);