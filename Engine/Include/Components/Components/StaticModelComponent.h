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

class StaticModelComponent final : public Component
{

	//Component declaration.
	CATALYST_COMPONENT
	(
		StaticModel,
		COMPONENT_INITIALIZE()
		COMPONENT_DEFAULT_INITIALIZATION_DATA(StaticModelInitializationData)
		COMPONENT_POST_CREATE_INSTANCE()
		COMPONENT_PARALLEL_BATCH_UPDATE(UpdatePhase::PRE_RENDER, 128)
		COMPONENT_SELECT()
		COMPONENT_START_GAME()
	);

public:

	/*
	*	Callback for before an editable field change happens.
	*/
	void PreEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

	/*
	*	Callback for after an editable field change happens.
	*/
	void PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT override;

private:

	/*
	*	Sets up default materials.
	*/
	void SetupDefaultMaterials(Entity *const RESTRICT entity, const bool force) NOEXCEPT;

};