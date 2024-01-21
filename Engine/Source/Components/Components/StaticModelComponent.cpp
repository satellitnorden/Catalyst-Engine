//Header file.
#include <Components/Components/StaticModelComponent.h>

//Rendering.
#include <Rendering/Native/Culling.h>
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/PhysicsSystem.h>

DEFINE_COMPONENT(StaticModelComponent, StaticModelGlobalData, StaticModelInitializationData, StaticModelInstanceData);

/*
*	Creates an instance.
*/
void StaticModelComponent::CreateInstance(const EntityIdentifier entity, void *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	StaticModelInitializationData *const RESTRICT _initialization_data{ static_cast<StaticModelInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	StaticModelInstanceData &instance_data{ _InstanceData.Back() };

	instance_data._ModelResource = _initialization_data->_ModelResource;
	instance_data._PreviousWorldTransform = instance_data._CurrentWorldTransform = _initialization_data->_InitialWorldTransform;
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(instance_data._ModelResource->_ModelSpaceAxisAlignedBoundingBox, _initialization_data->_InitialWorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
	instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);
	instance_data._MaterialResources = _initialization_data->_MaterialResources;
	instance_data._ModelCollisionConfiguration = _initialization_data->_ModelCollisionConfiguration;
	instance_data._ModelSimulationConfiguration = _initialization_data->_ModelSimulationConfiguration;
	instance_data._MeshesVisibleMask = UINT8_MAXIMUM;

	//Free the initialization data.
	FreeInitializationData(_initialization_data);

	//Create the actor.
	if (instance_data._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		instance_data._PhysicsActorHandle = nullptr;

		PhysicsSystem::Instance->CreateModelActor
		(
			instance_data._CurrentWorldTransform,
			instance_data._ModelCollisionConfiguration,
			instance_data._WorldSpaceAxisAlignedBoundingBox,
			instance_data._ModelResource->_CollisionModel,
			instance_data._ModelSimulationConfiguration,
			&instance_data._PhysicsActorHandle
		);
	}

	else
	{
		instance_data._PhysicsActorHandle = nullptr;
	}

	//Tell the ray tracing system.
	RenderingSystem::Instance->GetRayTracingSystem()->OnStaticModelInstanceCreated(entity, _InstanceData.Back());

	//Add the entity identifier.
	_EntityIdentifiers.Emplace(entity);

	//Add the entity to instance mapping.
	_EntityToInstanceMappings[entity] = _InstanceData.LastIndex();
}

/*
*	Destroys an instance.
*/
void StaticModelComponent::DestroyInstance(const EntityIdentifier entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ _EntityToInstanceMappings[entity] };

	//Cache the instance data.
	StaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy the physics actor.
	if (instance_data._PhysicsActorHandle)
	{
		PhysicsSystem::Instance->DestroyActor(&instance_data._PhysicsActorHandle);
	}

	//Tell the ray tracing system.
	RenderingSystem::Instance->GetRayTracingSystem()->OnStaticModelInstanceDestroyed(entity, instance_data);

	//Remove data.
	const EntityIdentifier moved_entity_identifier{ _EntityIdentifiers.Back() };
	_InstanceData.EraseAt<false>(instance_index);
	_EntityIdentifiers.EraseAt<false>(instance_index);
	_EntityToInstanceMappings[entity] = UINT64_MAXIMUM;
	_EntityToInstanceMappings[moved_entity_identifier] = instance_index;
}

void StaticModelComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE | UpdatePhase::PRE_RENDER;
	update_configuration->_BatchSize = 64;
}

void StaticModelComponent::Update(const UpdatePhase update_phase, const uint64 start_index, const uint64 end_index) NOEXCEPT
{
	switch (update_phase)
	{
		case UpdatePhase::PRE:
		{
			//Iterate over the instances.
			for (uint64 instance_index{ start_index }; instance_index < end_index; ++instance_index)
			{
				//Cache the instance data.
				StaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

				//Update the previous world transform.
				instance_data._PreviousWorldTransform = instance_data._CurrentWorldTransform;
			}
			
			break;
		}

		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
			const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_index }; instance_index < end_index; ++instance_index)
			{
				//Cache the instance data.
				StaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

				//Update the world transform is this static model instance is physics-simulated.
				if (instance_data._PhysicsActorHandle && instance_data._ModelSimulationConfiguration._SimulatePhysics)
				{
					PhysicsSystem::Instance->GetActorWorldTransform(instance_data._PhysicsActorHandle, &instance_data._CurrentWorldTransform);
				}

				//Cull.
				{
					//Reset the visibility flags.
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

					//Do camera culling.
					{
						//Do frustum culling.
						if (!Culling::IsWithinFrustum(instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA);
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