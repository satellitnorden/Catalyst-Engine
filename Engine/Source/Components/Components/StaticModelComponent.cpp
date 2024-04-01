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
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/DebugSystem.h>
#endif
#include <Systems/PhysicsSystem.h>
#include <Systems/ResourceSystem.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/WorldSystem.h>
#endif

#if !defined(CATALYST_CONFIGURATION_FINAL)
//Denotes whether or not static model wireframe is enabled.
bool STATIC_MODEL_WIREFRAME_ENABLED{ false };

/*
*	Static model wireframe push constant data.
*/
class StaticModelWireframePushConstantData final
{

public:

	//The transformation.
	Matrix4x4 _Transformation;

	//The color.
	Vector3<float32> _Color;

};

/*
*	Gathers a static model wireframe render input stream.
*/
void GatherStaticModelWireframeRenderInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Define constants.
	constexpr StaticArray<Vector3<float32>, 8> LEVEL_OF_DETAIL_COLORS
	{
		Vector3<float32>(1.0f, 0.0f, 0.0f),
		Vector3<float32>(0.0f, 1.0f, 0.0f),
		Vector3<float32>(0.0f, 0.0f, 1.0f),
		Vector3<float32>(1.0f, 1.0f, 0.0f),
		Vector3<float32>(1.0f, 0.0f, 1.0f),
		Vector3<float32>(0.0f, 1.0f, 1.0f),
		Vector3<float32>(1.0f, 1.0f, 1.0f),
		Vector3<float32>(0.0f, 0.0f, 0.0f)
	};

	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	if (STATIC_MODEL_WIREFRAME_ENABLED)
	{
		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < StaticModelComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			Entity *const RESTRICT entity{ StaticModelComponent::Instance->InstanceToEntity(instance_index) };
			const StaticModelInstanceData &static_model_instance_data{ StaticModelComponent::Instance->InstanceData(entity) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };
			 
			//Skip this model if it's not visible.
			if (!TEST_BIT(static_model_instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
			{
				continue;
			}

			//Go through all meshes.
			for (uint64 i{ 0 }, size{ static_model_instance_data._Model->_Meshes.Size() }; i < size; ++i)
			{
				//Cache the mesh.
				const Mesh &mesh{ static_model_instance_data._Model->_Meshes[i] };

				//Add a new entry.
				input_stream->_Entries.Emplace();
				RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

				new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
				new_entry._VertexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._VertexBuffer;
				new_entry._IndexBuffer = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexBuffer;
				new_entry._IndexBufferOffset = 0;
				new_entry._InstanceBuffer = EMPTY_HANDLE;
				new_entry._VertexCount = 0;
				new_entry._IndexCount = mesh._MeshLevelOfDetails[static_model_instance_data._LevelOfDetailIndices[i]]._IndexCount;
				new_entry._InstanceCount = 0;

				//Set up the push constant data.
				StaticModelWireframePushConstantData push_constant_data;

				push_constant_data._Transformation = world_transform_instance_data._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
				push_constant_data._Color = LEVEL_OF_DETAIL_COLORS[static_model_instance_data._LevelOfDetailIndices[i] % LEVEL_OF_DETAIL_COLORS.Size()];

				for (uint64 i{ 0 }; i < sizeof(StaticModelWireframePushConstantData); ++i)
				{
					input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
				}
			}
		}
	}
}
#endif

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

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Register the debug command.
	DebugSystem::Instance->RegisterCheckboxDebugCommand
	(
		"Rendering\\Static Model\\Wireframe",
		[](class DebugCommand *const RESTRICT debug_command, void *const RESTRICT user_data)
		{
			STATIC_MODEL_WIREFRAME_ENABLED = !STATIC_MODEL_WIREFRAME_ENABLED;
		},
		nullptr
	);

	//Register the input stream.
	{
		DynamicArray<VertexInputAttributeDescription> required_vertex_input_attribute_descriptions;

		required_vertex_input_attribute_descriptions.Emplace(0, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Position)));
		required_vertex_input_attribute_descriptions.Emplace(1, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Normal)));
		required_vertex_input_attribute_descriptions.Emplace(2, 0, VertexInputAttributeDescription::Format::X32Y32Z32SignedFloat, static_cast<uint32>(offsetof(Vertex, _Tangent)));
		required_vertex_input_attribute_descriptions.Emplace(3, 0, VertexInputAttributeDescription::Format::X32Y32SignedFloat, static_cast<uint32>(offsetof(Vertex, _TextureCoordinate)));

		DynamicArray<VertexInputBindingDescription> required_vertex_input_binding_descriptions;

		required_vertex_input_binding_descriptions.Emplace(0, static_cast<uint32>(sizeof(Vertex)), VertexInputBindingDescription::InputRate::Vertex);

		RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
		(
			HashString("StaticModelWireframe"),
			required_vertex_input_attribute_descriptions,
			required_vertex_input_binding_descriptions,
			sizeof(StaticModelWireframePushConstantData),
			[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
			{
				GatherStaticModelWireframeRenderInputStream(input_stream);
			},
			RenderInputStream::Mode::DRAW_INDEXED,
			nullptr
		);
	}
#endif
}

/*
*	Updates this component.
*/
void StaticModelComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
{
	//Define constants.
	constexpr float32 BASE_CULLING_DIMENSION_MULTIPLIER{ 64.0f };

	PROFILING_SCOPE("StaticModelComponent::ParallelBatchUpdate");

	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const WorldTransform &camera_world_transform{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform() };
			const Vector3<float32> camera_local_position{ camera_world_transform.GetLocalPosition() };
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

				//Otherwise, check if the world transform has updated, and if so, update the physics actor.
				else if (instance_data._PhysicsActorHandle && world_transform_instance_data._PreviousWorldTransform != world_transform_instance_data._CurrentWorldTransform)
				{
					PhysicsSystem::Instance->UpdateWorldTransform(world_transform_instance_data._CurrentWorldTransform, &instance_data._PhysicsActorHandle);
				}

				//Update the world space axis aligned bounding box.
				AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
				RenderingUtilities::TransformAxisAlignedBoundingBox(instance_data._Model->_ModelSpaceAxisAlignedBoundingBox, world_transform_instance_data._CurrentWorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
				instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
				instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);

				//Retrieve the relative axis aligned bounding box.
				const AxisAlignedBoundingBox3D relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_world_transform.GetCell()) };

				//Do culling.
				bool visible{ false };

				//Reset the visibility flags.
				instance_data._VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

				//Calculate the distance.
				const float32 distance{ Vector3<float32>::Length(AxisAlignedBoundingBox3D::GetClosestPointInside(relative_axis_aligned_bounding_box, camera_local_position) - camera_local_position) };

				/*
				*	Calculate the distance based culling score.
				*	Bias the height dimension a bit more, makes tall model popping is more noticable.
				*/
				const float32 culling_score
				{
					Culling::CalculateDistanceBasedCullingScore
					(
						relative_axis_aligned_bounding_box,
						Vector3<float32>(BASE_CULLING_DIMENSION_MULTIPLIER, BASE_CULLING_DIMENSION_MULTIPLIER * 2.0f, BASE_CULLING_DIMENSION_MULTIPLIER),
						distance
					)
				};

				//If the culling score is negative, just cull it outright.
				if (culling_score < 0.0f)
				{
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(0);
				}

				else
				{
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

							//Calculate the level of detail score.
							const float32 level_of_detail_score
							{
								1.0f - CatalystBaseMath::Maximum<float32>(Culling::CalculateDistanceBasedCullingScore
								(
									relative_axis_aligned_bounding_box,
									Vector3<float32>(instance_data._Model->_LevelOfDetailMultiplier, instance_data._Model->_LevelOfDetailMultiplier, instance_data._Model->_LevelOfDetailMultiplier),
									distance
								), 0.0f)
							};

							//Calculate the level of detail index.
							instance_data._LevelOfDetailIndices[mesh_index] = CatalystBaseMath::Minimum<uint8>(static_cast<uint8>(level_of_detail_score * static_cast<float32>(instance_data._Model->_Meshes[mesh_index]._MeshLevelOfDetails.Size())), static_cast<uint8>(instance_data._Model->_Meshes[mesh_index]._MeshLevelOfDetails.LastIndex()));
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
			static_model_instance_data._Model->_ModelSpaceAxisAlignedBoundingBox,
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
				instance_data._Model->_ModelSpaceAxisAlignedBoundingBox,
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