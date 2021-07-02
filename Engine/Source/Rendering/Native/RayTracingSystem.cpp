//Header file.
#include <Rendering/Native/RayTracingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Post-initializes the ray tracing system.
*/
void RayTracingSystem::PostInitialize()
{
	//Create the render data table layout.
	CreateRenderDataTableLayout();
}

/*
*	Updates the ray tracing system during the render update phase.
*/
void RayTracingSystem::RenderUpdate() NOEXCEPT
{
	//No need to do anything if ray tracing isn't active.
	if (!RenderingSystem::Instance->IsRayTracingActive())
	{
		return;
	}

	//Remember if anything has been updated.
	const bool anything_updated{ _DynamicModelsUpdated || _StaticModelsUpdated };

	//Perform updates.
	UpdateTerrain();
	UpdateStaticModels();
	UpdateDynamicModels();

	//Rebuild the render data table.
	if (anything_updated)
	{
		CreateRenderDataTable();
	}
}

/*
*	Sets the terrain bottom level acceleration structure.
*/
void RayTracingSystem::SetTerrainBottomLevelAccelerationStructure(const AccelerationStructureHandle handle) NOEXCEPT
{
	_TerrainBottomAccelerationStructure = handle;
}

/*
*	Creates the render data table layout.
*/
void RayTracingSystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Define constants.
	constexpr uint32 MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MESHES{ 1'024 * 4 }; //TODO: Figure out a way to remove this.
	constexpr uint32 MAXIMUM_NUMBER_OF_RAY_TRACED_DYNAMIC_MESHES{ 1'024 * 4 }; //TODO: Figure out a way to remove this.

	//Create the render data table layout.
	StaticArray<RenderDataTableLayoutBinding, 7> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_GENERATION | ShaderStage::RAY_MISS),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MESHES, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MESHES, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_DYNAMIC_MESHES, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_DYNAMIC_MESHES, ShaderStage::RAY_CLOSEST_HIT),
		RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RAY_CLOSEST_HIT)
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_RenderDataTableLayout);
}

/*
*	Creates the render data tables.
*/
void RayTracingSystem::CreateRenderDataTable() NOEXCEPT
{
	//Destroy the old render data table, if necessary.
	if (_RenderDataTable)
	{
		RenderingSystem::Instance->DestroyRenderDataTable(&_RenderDataTable);
		_RenderDataTable = EMPTY_HANDLE;
	}

	//Create the render data table.
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	//Destroy the old top level acceleration structure, if necessary.
	if (_TopLevelAccelerationStructure)
	{
		RenderingSystem::Instance->DestroyAccelerationStructure(&_TopLevelAccelerationStructure);
		_TopLevelAccelerationStructure = EMPTY_HANDLE;
	}

	//Fill up the top level acceleration structure instance data.
	_TopLevelAccelerationStructureInstanceData.Clear();

	for (const TopLevelAccelerationStructureInstanceData &data : _DynamicModelTopLevelAccelerationStructureInstanceData)
	{
		_TopLevelAccelerationStructureInstanceData.Emplace(data);
	}

	for (const TopLevelAccelerationStructureInstanceData &data : _StaticModelTopLevelAccelerationStructureInstanceData)
	{
		_TopLevelAccelerationStructureInstanceData.Emplace(data);
	}

	//No need to create anything if there's nothing to trace against.
	if (_TopLevelAccelerationStructureInstanceData.Empty())
	{
		return;
	}

	//Create the top level acceleration structure.
	RenderingSystem::Instance->CreateTopLevelAccelerationStructure(_TopLevelAccelerationStructureInstanceData, &_TopLevelAccelerationStructure);

	//Bind the top level acceleration structure to the render data tables.
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(0, 0, &_RenderDataTable, _TopLevelAccelerationStructure);
	
	//Bind all the static model data.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		uint32 mesh_counter{ 0 };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			for (const Mesh &mesh : component->_ModelResource->_Meshes)
			{
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(1, mesh_counter, &_RenderDataTable, mesh._MeshLevelOfDetails[0]._VertexBuffer);
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(2, mesh_counter, &_RenderDataTable, mesh._MeshLevelOfDetails[0]._IndexBuffer);

				++mesh_counter;
			}
		}

		if (_StaticModelsMaterialBuffer)
		{
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(3, 0, &_RenderDataTable, _StaticModelsMaterialBuffer);
		}
	}

	//Bind all the dynamic model data.
	{
		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		uint32 mesh_counter{ 0 };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			for (const Mesh &mesh : component->_ModelResource->_Meshes)
			{
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(4, mesh_counter, &_RenderDataTable, mesh._MeshLevelOfDetails[0]._VertexBuffer);
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(5, mesh_counter, &_RenderDataTable, mesh._MeshLevelOfDetails[0]._IndexBuffer);

				++mesh_counter;
			}
		}

		if (_DynamicModelsMaterialBuffer)
		{
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(6, 0, &_RenderDataTable, _DynamicModelsMaterialBuffer);
		}
	}
}

/*
*	Updates terrain.
*/
void RayTracingSystem::UpdateTerrain() NOEXCEPT
{
	//Add the terrain bottom level acceleration structure to the top level acceleration structure instance data.
	if (_TerrainBottomAccelerationStructure)
	{
		_TopLevelAccelerationStructureInstanceData.Emplace(MatrixConstants::IDENTITY, _TerrainBottomAccelerationStructure, RenderingConstants::TERRAIN_HIT_GROUP_INDEX, 0);
	}
}

/*
*	Updates dynamic models.
*/
void RayTracingSystem::UpdateDynamicModels() NOEXCEPT
{
	//Don't update dynamic models if they haven't been updated. (:
	if (!_DynamicModelsUpdated)
	{
		return;
	}

	//Reset whether or not stdynamicatic models have been updated.
	_DynamicModelsUpdated = false;

	//Clear the dynamic model top level acceleration instance data.
	_DynamicModelTopLevelAccelerationStructureInstanceData.Clear();

	//Destroy the old data.
	if (_DynamicModelsMaterialBuffer)
	{
		RenderingSystem::Instance->DestroyBuffer(&_DynamicModelsMaterialBuffer);
		_DynamicModelsMaterialBuffer = EMPTY_HANDLE;
	}

	//If there's no dynamic models left, no need to do anything.
	if (ComponentManager::GetNumberOfDynamicModelComponents() == 0)
	{
		return;
	}

	//Gather the instances and the material indices.
	_DynamicModelsMaterialindices.Clear();

	const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	uint32 mesh_counter{ 0 };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		for (uint64 j{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; j < size; ++j)
		{
			_DynamicModelsMaterialindices.Emplace(component->_MaterialResources[j]->_Index);
		}

		for (const Mesh &mesh : component->_ModelResource->_Meshes)
		{
			_DynamicModelTopLevelAccelerationStructureInstanceData.Emplace(TopLevelAccelerationStructureInstanceData(component->_CurrentWorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell()), mesh._MeshLevelOfDetails[0]._BottomLevelAccelerationStructure, RenderingConstants::DYNAMIC_MODELS_HIT_GROUP_INDEX, mesh_counter));

			++mesh_counter;
		}
	}

	//If the capacity is more than double the size, downsize the material indices buffer a bit to save memory. (:
	if (_DynamicModelsMaterialindices.Capacity() > _DynamicModelsMaterialindices.Size() * 2)
	{
		_DynamicModelsMaterialindices.Reserve(_DynamicModelsMaterialindices.Size());
	}

	//Create the dynamic models material buffer.
	RenderingSystem::Instance->CreateBuffer(sizeof(uint32) * _DynamicModelsMaterialindices.Size(), BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &_DynamicModelsMaterialBuffer);

	const void* RESTRICT data_chunks[]{ _DynamicModelsMaterialindices.Data() };
	const uint64 data_sizes[]{ sizeof(uint32) * _DynamicModelsMaterialindices.Size() };
	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_DynamicModelsMaterialBuffer);
}

/*
*	Updates static models.
*/
void RayTracingSystem::UpdateStaticModels() NOEXCEPT
{
	//Don't update static models if they haven't been updated. (:
	if (!_StaticModelsUpdated)
	{
		return;
	}

	//Reset whether or not static models have been updated.
	_StaticModelsUpdated = false;

	//Clear the static model top level acceleration instance data.
	_StaticModelTopLevelAccelerationStructureInstanceData.Clear();

	//Destroy the old data.
	if (_StaticModelsMaterialBuffer)
	{
		RenderingSystem::Instance->DestroyBuffer(&_StaticModelsMaterialBuffer);
		_StaticModelsMaterialBuffer = EMPTY_HANDLE;
	}

	//If there's no static models left, no need to do anything.
	if (ComponentManager::GetNumberOfStaticModelComponents() == 0)
	{
		return;
	}

	//Gather the instances and the material indices.
	_StaticModelsMaterialindices.Clear();

	const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	uint32 mesh_counter{ 0 };

	for (uint64 component_index{ 0 }; component_index < number_of_components; ++component_index, ++component)
	{
		for (uint64 mesh_index{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
		{
			if (!TEST_BIT(component->_MeshesVisibleMask, BIT(mesh_index)))
			{
				continue;
			}

			_StaticModelsMaterialindices.Emplace(component->_MaterialResources[mesh_index]->_Index);

			_StaticModelTopLevelAccelerationStructureInstanceData.Emplace(TopLevelAccelerationStructureInstanceData(component->_WorldTransform.ToRelativeMatrix4x4(WorldSystem::Instance->GetCurrentWorldGridCell()), component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails[0]._BottomLevelAccelerationStructure, RenderingConstants::STATIC_MODELS_HIT_GROUP_INDEX, mesh_counter));

			++mesh_counter;
		}
	}

	//If the capacity is more than double the size, downsize the material indices buffer a bit to save memory. (:
	if (_StaticModelsMaterialindices.Capacity() > _StaticModelsMaterialindices.Size() * 2)
	{
		_StaticModelsMaterialindices.Reserve(_StaticModelsMaterialindices.Size());
	}

	//Create the static models material buffer.
	RenderingSystem::Instance->CreateBuffer(sizeof(uint32) * _StaticModelsMaterialindices.Size(), BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &_StaticModelsMaterialBuffer);

	const void* RESTRICT data_chunks[]{ _StaticModelsMaterialindices.Data() };
	const uint64 data_sizes[]{ sizeof(uint32) * _StaticModelsMaterialindices.Size() };
	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_StaticModelsMaterialBuffer);
}