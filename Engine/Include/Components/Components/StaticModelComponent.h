#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Components.
#include <Components/Core/Component.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/MaterialAsset.h>
#include <Content/Assets/ModelAsset.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>
#include <Physics/Native/ModelSimulationConfiguration.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>

class StaticModelInitializationData final : public ComponentInitializationData
{

public:

	//The model.
	AssetPointer<ModelAsset> _Model;

	//The materials.
	StaticArray<AssetPointer<MaterialAsset>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _Materials;

	//The collision type.
	ModelCollisionType _CollisionType;

	//The model simulation configuration.
	ModelSimulationConfiguration _ModelSimulationConfiguration;

};

class StaticModelInstanceData final
{

public:

	//The model.
	AssetPointer<ModelAsset> _Model;

	//The world space axis aligned bounding box.
	WorldSpaceAxisAlignedBoundingBox3D _WorldSpaceAxisAlignedBoundingBox;

	//The materials.
	StaticArray<AssetPointer<MaterialAsset>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _Materials;

	//The physics actor handle.
	ActorHandle _PhysicsActorHandle;

	//The collision type.
	ModelCollisionType _CollisionType;

	//The model simulation configuration.
	ModelSimulationConfiguration _ModelSimulationConfiguration;

	//The level of detail indices.
	StaticArray<uint8, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _LevelOfDetailIndices;

	//Bitmask for the visible meshes.
	uint8 _MeshesVisibleMask;

	//The visibility flags.
	VisibilityFlags _VisibilityFlags;

};

DECLARE_COMPONENT
(
	StaticModelComponent,
	StaticModelInitializationData,
	StaticModelInstanceData,
	COMPONENT_INITIALIZE()
	COMPONENT_PARALLEL_BATCH_UPDATE(UpdatePhase::PRE_RENDER, 128)

public:

	/*
	*	Runs after all components have created their instance for the given entity.
	*	Useful if there is some setup needed involving multiple components.
	*/
	void PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT override;

	/*
	*	Sets default values for initialization data.
	*/
	void DefaultInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT override;

	/*
	*	Callback for before an editable field change happens.
	*/
	void PreEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

	/*
	*	Callback for after an editable field change happens.
	*/
	void PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

);