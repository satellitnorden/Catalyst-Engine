//Header file.
#include <Rendering/Native/RayTracingSystem.h>

//Components.
#include <Components/Components/StaticModelComponent.h>
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

//STL.
#include <stdio.h>

/*
*	Post-initializes the ray tracing system.
*/
void RayTracingSystem::PostInitialize()
{
	//Add the hit groups.
	{
		_HitGroups.Emplace();
		RayTracingHitGroup &new_hit_group{ _HitGroups.Back() };

		new_hit_group._Identifier = HashString("OpaqueModels");
		new_hit_group._Index = static_cast<uint32>(_HitGroups.LastIndex());
		new_hit_group._BottomLevelAccelerationStructureFlags = BottomLevelAccelerationStructureFlag::OPAQUE;
	}

	{
		_HitGroups.Emplace();
		RayTracingHitGroup &new_hit_group{ _HitGroups.Back() };

		new_hit_group._Identifier = HashString("MaskedModels");
		new_hit_group._Index = static_cast<uint32>(_HitGroups.LastIndex());
		new_hit_group._BottomLevelAccelerationStructureFlags = BottomLevelAccelerationStructureFlag::NONE;
	}

	//Create the render data table layout.
	CreateRenderDataTableLayout();

	//Create the render data tables.
	_RenderDataTables.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (RenderDataTableHandle &render_data_table : _RenderDataTables)
	{
		RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &render_data_table);
	}

	//Create the top level acceleration structures.
	_TopLevelAccelerationStructures.Upsize<false>(RenderingSystem::Instance->GetNumberOfFramebuffers());

	for (AccelerationStructureHandle &top_level_acceleration_structure : _TopLevelAccelerationStructures)
	{
		top_level_acceleration_structure = EMPTY_HANDLE;
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(), &top_level_acceleration_structure, nullptr);
	}
}

/*
*	Updates the ray tracing system during the render update phase.
*/
void RayTracingSystem::RenderUpdate(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT
{
	//No need to do anything if ray tracing isn't active.
	if (!RenderingSystem::Instance->IsRayTracingActive())
	{
		return;
	}

	//Cache the current render data table.
	RenderDataTableHandle &current_render_data_table{ _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Cache the current top level acceleration structure.
	AccelerationStructureHandle &current_top_level_acceleration_structure{ _TopLevelAccelerationStructures[RenderingSystem::Instance->GetCurrentFramebufferIndex()] };

	//Gather the instance data, and update the current render data table at the same time.
	{
		PROFILING_SCOPE("RayTracingSystem_GatherInstanceData");

		_TopLevelAccelerationStructureInstanceData.Clear();

		for (uint64 hit_group_index{ 0 }; hit_group_index < _HitGroups.Size(); ++hit_group_index)
		{
			RayTracingHitGroup &hit_group{ _HitGroups[hit_group_index] };

			hit_group._Materialindices.Clear();

			for (uint64 entry_index{ 0 }; entry_index < hit_group._Entries.Size(); ++entry_index)
			{
				const RayTracingHitGroup::Entry &entry{ hit_group._Entries[entry_index] };

				_TopLevelAccelerationStructureInstanceData.Emplace(entry._InstanceData);

				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(static_cast<uint32>(1 + hit_group_index * 3 + 0), static_cast<uint32>(entry_index), &current_render_data_table, entry._VertexBuffer);
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(static_cast<uint32>(1 + hit_group_index * 3 + 1), static_cast<uint32>(entry_index), &current_render_data_table, entry._IndexBuffer);
			
				hit_group._Materialindices.Emplace(entry._MaterialIndex);
			}

			//Does the material indices buffer need to be (re-)created?
			if (hit_group._MaterialBufferIndicesCapacity < hit_group._Materialindices.Size())
			{
				if (hit_group._MaterialIndicesBuffer != EMPTY_HANDLE)
				{
					RenderingSystem::Instance->DestroyBuffer(&hit_group._MaterialIndicesBuffer);
				}

				RenderingSystem::Instance->CreateBuffer(sizeof(uint32) * hit_group._Materialindices.Size(), BufferUsage::StorageBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &hit_group._MaterialIndicesBuffer);

				const void *RESTRICT data_chunks[]{ hit_group._Materialindices.Data() };
				const uint64 data_sizes[]{ sizeof(uint32) * hit_group._Materialindices.Size() };
				RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &hit_group._MaterialIndicesBuffer);

				hit_group._MaterialBufferIndicesCapacity = hit_group._Materialindices.Size();
			}

			if (!hit_group._Materialindices.Empty())
			{
 				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(static_cast<uint32>(1 + hit_group_index * 3 + 2), 0, &current_render_data_table, hit_group._MaterialIndicesBuffer);
			}
		}
	}

	{
		PROFILING_SCOPE("RayTracingSystem_BuildTopLevelAccelerationStructure");
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(_TopLevelAccelerationStructureInstanceData, &current_top_level_acceleration_structure, command_buffer);
	}

	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(0, 0, &current_render_data_table, current_top_level_acceleration_structure);
}

/*
*	Returns the current render data table.
*/
NO_DISCARD RenderDataTableHandle RayTracingSystem::GetCurrentRenderDataTable() NOEXCEPT
{
	return _RenderDataTables[RenderingSystem::Instance->GetCurrentFramebufferIndex()];
}

/*
*	Callback for when a static model instance is created.
*/
void RayTracingSystem::OnStaticModelInstanceCreated(Entity *const RESTRICT entity, const StaticModelInstanceData &instance_data) NOEXCEPT
{
	//Iterate through the meshes.
	for (uint64 mesh_index{ 0 }, size{ instance_data._Model->_Meshes.Size() }; mesh_index < size; ++mesh_index)
	{
		//Cache the mesh.
		const Mesh &mesh{ instance_data._Model->_Meshes[mesh_index] };

		//Cache the mesh level of detail.
		const Mesh::MeshLevelOfDetail &mesh_level_of_detail{ mesh._MeshLevelOfDetails[0] };

		//Decide which hit group this mesh goes in.
		RayTracingHitGroup *RESTRICT hit_group;

		if (instance_data._Materials[mesh_index]->_Type == MaterialAsset::Type::MASKED)
		{
			hit_group = GetHitGroup(HashString("MaskedModels"));
		}

		else
		{
			hit_group = GetHitGroup(HashString("OpaqueModels"));
		}

		//Calculate a unique identifier for this mesh.
		char mesh_identifier_buffer[64];
		sprintf_s(mesh_identifier_buffer, "%s_%llu", instance_data._Model->_Header._AssetName.Data(), mesh_index);

		const HashString mesh_identifier{ mesh_identifier_buffer };

		//Retrieve or create the bottom level acceleration structure.
		AccelerationStructureHandle bottom_level_acceleration_structure;

		if (RayTracingHitGroup::SharedBottomLevelAccelerationStructure *const RESTRICT shared_bottom_level_acceleration_structure{ hit_group->_SharedBottomLevelAccelerationStructures.Find(mesh_identifier) })
		{
			bottom_level_acceleration_structure = shared_bottom_level_acceleration_structure->_BottomLevelAccelerationStructure;
			++shared_bottom_level_acceleration_structure->_ReferenceCount;
		}

		else
		{
			RayTracingHitGroup::SharedBottomLevelAccelerationStructure new_shared_bottom_level_acceleration_structure;

			RenderingSystem::Instance->CreateBottomLevelAccelerationStructure
			(
				mesh_level_of_detail._VertexBuffer,
				static_cast<uint32>(mesh_level_of_detail._Vertices.Size()),
				mesh_level_of_detail._IndexBuffer,
				mesh_level_of_detail._IndexCount,
				hit_group->_BottomLevelAccelerationStructureFlags,
				&new_shared_bottom_level_acceleration_structure._BottomLevelAccelerationStructure
			);

			new_shared_bottom_level_acceleration_structure._ReferenceCount = 1;

			hit_group->_SharedBottomLevelAccelerationStructures.Add(mesh_identifier, new_shared_bottom_level_acceleration_structure);

			bottom_level_acceleration_structure = new_shared_bottom_level_acceleration_structure._BottomLevelAccelerationStructure;
		}

		//Add a new entry.
		hit_group->_Entries.Emplace();
		RayTracingHitGroup::Entry &new_entry{ hit_group->_Entries.Back() };

		new_entry._EntityIdentifier = entity->_EntityIdentifier;
		new_entry._VertexBuffer = mesh_level_of_detail._VertexBuffer;
		new_entry._IndexBuffer = mesh_level_of_detail._IndexBuffer;
		new_entry._MaterialIndex = instance_data._Materials[mesh_index]->_Index;
		new_entry._InstanceData._Transform = WorldTransformComponent::Instance->InstanceData(entity)._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());
		new_entry._InstanceData._BottomLevelAccelerationStructure = bottom_level_acceleration_structure;
		new_entry._InstanceData._HitGroupIndex = hit_group->_Index;
		new_entry._InstanceData._InstanceIndex = static_cast<uint32>(hit_group->_Entries.LastIndex());
	}
}

/*
*	Updates the world transform for the given static model instance.
*/
void RayTracingSystem::UpdateStaticModelInstanceWorldTransform(Entity* const RESTRICT entity, const StaticModelInstanceData &instance_data) NOEXCEPT
{
	//Iterate through the meshes.
	for (uint64 mesh_index{ 0 }, size{ instance_data._Model->_Meshes.Size() }; mesh_index < size; ++mesh_index)
	{
		//Decide which hit group this mesh is in.
		RayTracingHitGroup *RESTRICT hit_group;

		if (instance_data._Materials[mesh_index]->_Type == MaterialAsset::Type::MASKED)
		{
			hit_group = GetHitGroup(HashString("MaskedModels"));
		}

		else
		{
			hit_group = GetHitGroup(HashString("OpaqueModels"));
		}

		//Remove the entries associated with this entity.
		for (RayTracingHitGroup::Entry &entry : hit_group->_Entries)
		{
			if (entry._EntityIdentifier == entity->_EntityIdentifier)
			{
				entry._InstanceData._Transform = WorldTransformComponent::Instance->InstanceData(entity)._CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell());

				break;
			}
		}
	}
}

/*
*	Callback for when a static model instance is destroyed.
*/
void RayTracingSystem::OnStaticModelInstanceDestroyed(Entity *const RESTRICT entity, const StaticModelInstanceData &instance_data) NOEXCEPT
{
	//Iterate through the meshes.
	for (uint64 mesh_index{ 0 }, size{ instance_data._Model->_Meshes.Size() }; mesh_index < size; ++mesh_index)
	{
		//Decide which hit group this mesh is in.
		RayTracingHitGroup *RESTRICT hit_group;

		if (instance_data._Materials[mesh_index]->_Type == MaterialAsset::Type::MASKED)
		{
			hit_group = GetHitGroup(HashString("MaskedModels"));
		}

		else
		{
			hit_group = GetHitGroup(HashString("OpaqueModels"));
		}

		//Remove the entries associated with this entity.
		for (uint64 i{ 0 }; i < hit_group->_Entries.Size();)
		{
			if (hit_group->_Entries[i]._EntityIdentifier == entity->_EntityIdentifier)
			{
				hit_group->_Entries.EraseAt<false>(i);

				if (i < hit_group->_Entries.Size())
				{
					hit_group->_Entries[i]._InstanceData._InstanceIndex = static_cast<uint32>(i);
				}
			}

			else
			{
				++i;
			}
		}

		//Calculate a unique identifier for this mesh.
		char mesh_identifier_buffer[64];
		sprintf_s(mesh_identifier_buffer, "%s_%llu", instance_data._Model->_Header._AssetName.Data(), mesh_index);

		const HashString mesh_identifier{ mesh_identifier_buffer };

		//Decrement the reference count for the shared bottom level acceleration structures. Also, destroy them if they are no longer in use.
		if (RayTracingHitGroup::SharedBottomLevelAccelerationStructure *const RESTRICT shared_bottom_level_acceleration_structure{ hit_group->_SharedBottomLevelAccelerationStructures.Find(mesh_identifier) })
		{
			--shared_bottom_level_acceleration_structure->_ReferenceCount;

			if (shared_bottom_level_acceleration_structure->_ReferenceCount == 0)
			{
				RenderingSystem::Instance->DestroyAccelerationStructure(&shared_bottom_level_acceleration_structure->_BottomLevelAccelerationStructure);

				hit_group->_SharedBottomLevelAccelerationStructures.Remove(mesh_identifier);
			}
		}
	}
}

/*
*	Returns the hit group with the given identifier.
*/
NO_DISCARD RayTracingHitGroup *const RESTRICT RayTracingSystem::GetHitGroup(const HashString identifier) NOEXCEPT
{
	for (RayTracingHitGroup &hit_group : _HitGroups)
	{
		if (hit_group._Identifier == identifier)
		{
			return &hit_group;
		}
	}

	ASSERT(false, "Couldn't find hit group with identifier!");

	return nullptr;
}

/*
*	Creates the render data table layout.
*/
void RayTracingSystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Define constants.
	constexpr uint32 MAXIMUM_NUMBER_OF_RAY_TRACED_MESHES{ 1'024 * 4 }; //TODO: Figure out a way to remove this.

	//Create the render data table layout.
	DynamicArray<RenderDataTableLayoutBinding> bindings;

	bindings.Emplace(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS);

	for (const RayTracingHitGroup &hit_group : _HitGroups)
	{
		bindings.Emplace(static_cast<uint32>(bindings.Size()), RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_MESHES, ShaderStage::RAY_ANY_HIT | ShaderStage::RAY_CLOSEST_HIT);
		bindings.Emplace(static_cast<uint32>(bindings.Size()), RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_MESHES, ShaderStage::RAY_ANY_HIT | ShaderStage::RAY_CLOSEST_HIT);
		bindings.Emplace(static_cast<uint32>(bindings.Size()), RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RAY_ANY_HIT | ShaderStage::RAY_CLOSEST_HIT);
	}

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}