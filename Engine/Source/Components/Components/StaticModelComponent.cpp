//Header file.
#include <Components/Components/StaticModelComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Culling.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/ResourceSystem.h>

DEFINE_COMPONENT(StaticModelComponent, StaticModelInitializationData, StaticModelInstanceData);

/*
*	Initializes this component.
*/
void StaticModelComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableModelAssetField
	(
		"Model",
		offsetof(StaticModelInitializationData, _Model),
		offsetof(StaticModelInstanceData, _Model)
	);

	AddEditableMaterialAssetField
	(
		"Material 1",
		offsetof(StaticModelInitializationData, _Materials) + sizeof(AssetPointer<MaterialAsset>) * 0,
		offsetof(StaticModelInstanceData, _Materials) + sizeof(AssetPointer<MaterialAsset>) * 0
	);

	AddEditableMaterialAssetField
	(
		"Material 2",
		offsetof(StaticModelInitializationData, _Materials) + sizeof(AssetPointer<MaterialAsset>) * 1,
		offsetof(StaticModelInstanceData, _Materials) + sizeof(AssetPointer<MaterialAsset>) * 1
	);

	AddEditableEnumerationField
	(
		"Collision Type",
		offsetof(StaticModelInitializationData, _CollisionType),
		offsetof(StaticModelInstanceData, _CollisionType)
	);
}

/*
*	Post-initializes this component.
*/
void StaticModelComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void StaticModelComponent::Terminate() NOEXCEPT
{

}

void StaticModelComponent::DefaultInitializationData(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	StaticModelInitializationData *const RESTRICT _initialization_data{ static_cast<StaticModelInitializationData* const RESTRICT>(initialization_data) };

	_initialization_data->_Model = ContentSystem::Instance->GetAsset<ModelAsset>(HashString("Cube"));

	for (uint32 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
	{
		_initialization_data->_Materials[i] = ContentSystem::Instance->GetAsset<MaterialAsset>(HashString("Default"));
	}
	
	_initialization_data->_CollisionType = ModelCollisionType::NONE;
	_initialization_data->_ModelSimulationConfiguration._SimulatePhysics = false;
}

NO_DISCARD bool StaticModelComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void StaticModelComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void StaticModelComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	StaticModelInitializationData *const RESTRICT _initialization_data{ static_cast<StaticModelInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	StaticModelInstanceData &instance_data{ _InstanceData.Back() };

	instance_data._Model = _initialization_data->_Model;
	instance_data._Materials = _initialization_data->_Materials;
	instance_data._CollisionType = _initialization_data->_CollisionType;
	instance_data._ModelSimulationConfiguration = _initialization_data->_ModelSimulationConfiguration;
	instance_data._MeshesVisibleMask = UINT8_MAXIMUM;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void StaticModelComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Static model component needs a world transform component!");

	//Cache the instance data.
	const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };
	StaticModelInstanceData &static_model_instance_data{ InstanceData(entity) };

	//Calculate the world space axis aligned bounding box.
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(static_model_instance_data._Model->_ModelSpaceAxisAlignedBoundingBox, world_transform_instance_data._CurrentWorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	static_model_instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
	static_model_instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);

	//Tell the ray tracing system.
	RenderingSystem::Instance->GetRayTracingSystem()->OnStaticModelInstanceCreated(entity, _InstanceData.Back());

	//Create the physics actor.
	if (static_model_instance_data._CollisionType != ModelCollisionType::NONE)
	{
		static_model_instance_data._PhysicsActorHandle = nullptr;

		PhysicsSystem::Instance->CreateModelActor
		(
			world_transform_instance_data._CurrentWorldTransform,
			static_model_instance_data._CollisionType,
			static_model_instance_data._WorldSpaceAxisAlignedBoundingBox,
			static_model_instance_data._Model->_CollisionModel,
			static_model_instance_data._ModelSimulationConfiguration,
			&static_model_instance_data._PhysicsActorHandle
		);
	}

	else
	{
		static_model_instance_data._PhysicsActorHandle = nullptr;
	}
}

/*
*	Destroys an instance.
*/
void StaticModelComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	StaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy the physics actor.
	if (instance_data._PhysicsActorHandle)
	{
		PhysicsSystem::Instance->DestroyActor(&instance_data._PhysicsActorHandle);
	}

	//Tell the ray tracing system.
	RenderingSystem::Instance->GetRayTracingSystem()->OnStaticModelInstanceDestroyed(entity, instance_data);

	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 StaticModelComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void StaticModelComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE_RENDER;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 128;
}

/*
*	Runs before the given update phase.
*/
void StaticModelComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void StaticModelComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("StaticModelComponent::Update");

	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const WorldTransform &camera_world_transform{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform() };
			const Vector3<int32> camera_cell{ camera_world_transform.GetCell() };
			const Matrix4x4 *const RESTRICT camera_world_to_clip_matrix{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetViewMatrix() };
			const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
			{
				//Cache the instance data.
				StaticModelInstanceData &instance_data{ _InstanceData[instance_index] };
				WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(InstanceToEntity(instance_index)) };

				//Update the world transform is this static model instance is physics-simulated.
				if (instance_data._PhysicsActorHandle && instance_data._ModelSimulationConfiguration._SimulatePhysics)
				{
					PhysicsSystem::Instance->GetActorWorldTransform(instance_data._PhysicsActorHandle, &world_transform_instance_data._CurrentWorldTransform);
				}

				//Update the world space axis aligned bounding box.
				AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
				RenderingUtilities::TransformAxisAlignedBoundingBox(instance_data._Model->_ModelSpaceAxisAlignedBoundingBox, world_transform_instance_data._CurrentWorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
				instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
				instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);

				//Retrieve the relative axis aligned bounding box.
				const AxisAlignedBoundingBox3D relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_world_transform.GetCell()) };

				//Calculate the screen coverage.
				const float32 screen_coverage{ Culling::CalculateScreenCoverage(relative_axis_aligned_bounding_box, *camera_world_to_clip_matrix) };

				//If the screen coverage is really low, just cull it outright.
				if (screen_coverage <= 0.0001f)
				{
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(0);
				}

				else
				{
					//Do culling.
					bool visible{ false };

					//Reset the visibility flags.
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

					//Do camera culling.
					{
						//Do frustum culling.
						if (!Culling::IsWithinFrustum(instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA);
						}

						else
						{
							visible = true;
						}
					}

					//Do shadow map culling.
					for (uint32 shadow_map_data_index{ 0 }; shadow_map_data_index < RenderingSystem::Instance->GetShadowsSystem()->GetNumberOfShadowMapData(); ++shadow_map_data_index)
					{
						const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(shadow_map_data_index) };

						//Do frustum culling.
						if (!Culling::IsWithinFrustum(instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), shadow_map_data._Frustum))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::SHADOW_MAP_START << shadow_map_data_index);
						}

						else
						{
							visible = true;
						}
					}

					//Do level of detail.
					if (visible)
					{
						for (uint64 mesh_index{ 0 }, size{ instance_data._Model->_Meshes.Size() }; mesh_index < size; ++mesh_index)
						{
							//If the mesh used only has one level of detail, skip it.
							if (instance_data._Model->_Meshes[mesh_index]._MeshLevelOfDetails.Size() == 1)
							{
								instance_data._LevelOfDetailIndices[mesh_index] = 0;

								continue;
							}

							//Calculate the level of detail index.
							instance_data._LevelOfDetailIndices[mesh_index] = static_cast<uint32>((1.0f - screen_coverage) * static_cast<float32>(instance_data._Model->_Meshes[mesh_index]._MeshLevelOfDetails.Size() - 1));
						}
					}
				}
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Runs after the given update phase.
*/
void StaticModelComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

void StaticModelComponent::PreEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
{	
	//Cache the instance data.
	StaticModelInstanceData &instance_data{ InstanceData(entity) };

	if (editable_field._Identifier == HashString("Collision Type"))
	{
		//Destroy the physics actor.
		if (instance_data._PhysicsActorHandle)
		{
			PhysicsSystem::Instance->DestroyActor(&instance_data._PhysicsActorHandle);
		}
	}
}

void StaticModelComponent::PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
{
	//Cache the instance data.
	StaticModelInstanceData &instance_data{ InstanceData(entity) };

	if (editable_field._Identifier == HashString("Collision Type"))
	{
		//Create the physics actor.
		if (instance_data._CollisionType != ModelCollisionType::NONE)
		{
			//Cache the world transform instance data.
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			instance_data._PhysicsActorHandle = nullptr;

			PhysicsSystem::Instance->CreateModelActor
			(
				world_transform_instance_data._CurrentWorldTransform,
				instance_data._CollisionType,
				instance_data._WorldSpaceAxisAlignedBoundingBox,
				instance_data._Model->_CollisionModel,
				instance_data._ModelSimulationConfiguration,
				&instance_data._PhysicsActorHandle
			);
		}

		else
		{
			instance_data._PhysicsActorHandle = nullptr;
		}
	}
}