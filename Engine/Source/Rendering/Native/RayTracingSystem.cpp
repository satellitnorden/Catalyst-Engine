//Header file.
#include <Rendering/Native/RayTracingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Post-initializes the ray tracing system.
*/
void RayTracingSystem::PostInitialize()
{
	//Create the empty top level acceleration structure.
	CreateEmptyTopLevelAccelerationStructure();

	//Create the render data table layout.
	CreateRenderDataTableLayout();
}

/*
*	Updates the ray tracing system during the render update phase.
*/
void RayTracingSystem::RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//No need to do anything if ray tracing isn't active.
	if (!RenderingSystem::Instance->IsRayTracingActive())
	{
		return;
	}

	//Perform updates.
	if (_TerrainNeedsUpdate)
	{
		UpdateTerrain();
	}

	if (_StaticModelsNeedsUpdate)
	{
		UpdateStaticModels();
	}

	if (_DynamicModelsNeedsUpdate)
	{
		UpdateDynamicModels();
	}

	//Does the render data table need to be updated?
	if (_TerrainNeedsUpdate || _StaticModelsNeedsUpdate || _DynamicModelsNeedsUpdate)
	{
		//Rebuild the render data table.
		CreateRenderDataTable();

		//Reset the state.
		_TerrainNeedsUpdate = false;
		_StaticModelsNeedsUpdate = false;
		_DynamicModelsNeedsUpdate = false;
	}
}

/*
*	Sets the terrain top level acceleration structure.
*/
void RayTracingSystem::SetTerrainTopLevelAccelerationStructure(const AccelerationStructureHandle handle) NOEXCEPT
{
	_TerrainTopAccelerationStructure = handle;
	_TerrainNeedsUpdate = true;
}

/*
*	Notifies the ray tracing system that a static model was initialized.
*/
void RayTracingSystem::NofityStaticModelInitialized() NOEXCEPT
{
	_StaticModelsNeedsUpdate = true;
}

/*
*	Notifies the ray tracing system that a static model was terminated.
*/
void RayTracingSystem::NofityStaticModelTerminated() NOEXCEPT
{
	_StaticModelsNeedsUpdate = true;
}

/*
*	Notifies the ray tracing system that a dynamic model was initialized.
*/
void RayTracingSystem::NofityDynamicModelInitialized() NOEXCEPT
{
	_DynamicModelsNeedsUpdate = true;
}

/*
*	Notifies the ray tracing system that a dynamic model was modified.
*/
void RayTracingSystem::NofityDynamicModelModified() NOEXCEPT
{
	_DynamicModelsNeedsUpdate = true;
}

/*
*	Notifies the ray tracing system that a dynamic model was terminated.
*/
void RayTracingSystem::NofityDynamicModelTerminated() NOEXCEPT
{
	_DynamicModelsNeedsUpdate = true;
}

/*
*	Creates the empty top level acceleration structure.
*/
void RayTracingSystem::CreateEmptyTopLevelAccelerationStructure() NOEXCEPT
{
	RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(), &_EmptyTopLevelAccelerationStructure);
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
	StaticArray<RenderDataTableLayoutBinding, 9> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MESHES, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MESHES, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(5, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(6, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_DYNAMIC_MESHES, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(7, RenderDataTableLayoutBinding::Type::StorageBuffer, MAXIMUM_NUMBER_OF_RAY_TRACED_DYNAMIC_MESHES, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(8, RenderDataTableLayoutBinding::Type::StorageBuffer, 1, ShaderStage::RayClosestHit),
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
	}

	//Create the render data table.
	RenderingSystem::Instance->CreateRenderDataTable(_RenderDataTableLayout, &_RenderDataTable);

	//Bind the terrain top level acceleration structure to the render data tables.
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(0, 0, &_RenderDataTable, _TerrainTopAccelerationStructure ? _TerrainTopAccelerationStructure : _EmptyTopLevelAccelerationStructure);
	
	//Bind all the static model data.
	{
		RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(1, 0, &_RenderDataTable, _StaticModelsTopLevelAccelerationStructure ? _StaticModelsTopLevelAccelerationStructure : _EmptyTopLevelAccelerationStructure);

		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		uint32 mesh_counter{ 0 };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			for (const Mesh &mesh : component->_Model->_Meshes)
			{
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(2, mesh_counter, &_RenderDataTable, mesh._VertexBuffers[0]);
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(3, mesh_counter, &_RenderDataTable, mesh._IndexBuffers[0]);

				++mesh_counter;
			}
		}

		if (_StaticModelsMaterialBuffer)
		{
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(4, 0, &_RenderDataTable, _StaticModelsMaterialBuffer);
		}
	}

	//Bind all the dynamic model data.
	{
		RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(5, 0, &_RenderDataTable, _DynamicModelsTopLevelAccelerationStructure ? _DynamicModelsTopLevelAccelerationStructure : _EmptyTopLevelAccelerationStructure);

		const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
		const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

		uint32 mesh_counter{ 0 };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			for (const Mesh &mesh : component->_Model->_Meshes)
			{
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(6, mesh_counter, &_RenderDataTable, mesh._VertexBuffers[0]);
				RenderingSystem::Instance->BindStorageBufferToRenderDataTable(7, mesh_counter, &_RenderDataTable, mesh._IndexBuffers[0]);

				++mesh_counter;
			}
		}

		if (_DynamicModelsMaterialBuffer)
		{
			RenderingSystem::Instance->BindStorageBufferToRenderDataTable(8, 0, &_RenderDataTable, _DynamicModelsMaterialBuffer);
		}
	}
}

/*
*	Updates terrain.
*/
void RayTracingSystem::UpdateTerrain() NOEXCEPT
{
	//Nothing to do here right now.
}

/*
*	Updates static models.
*/
void RayTracingSystem::UpdateStaticModels() NOEXCEPT
{
	//Destroy the old data.
	if (_StaticModelsTopLevelAccelerationStructure)
	{
		RenderingSystem::Instance->DestroyAccelerationStructure(&_StaticModelsTopLevelAccelerationStructure);
		_StaticModelsTopLevelAccelerationStructure = EMPTY_HANDLE;
	}

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
	DynamicArray<TopLevelAccelerationStructureInstanceData> instances;
	DynamicArray<uint32> material_indices;

	const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	uint32 mesh_counter{ 0 };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		for (const uint32 material_index : component->_MaterialIndices)
		{
			material_indices.Emplace(material_index);
		}

		for (const Mesh &mesh : component->_Model->_Meshes)
		{
			instances.Emplace(TopLevelAccelerationStructureInstanceData(component->_WorldTransform, mesh._BottomLevelAccelerationStructure, RenderingConstants::STATIC_MODELS_HIT_GROUP_INDEX, mesh_counter));

			++mesh_counter;
		}
	}

	//Create the top level acceleration structure.
	RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &_StaticModelsTopLevelAccelerationStructure);

	//Create the dynamic models material buffer.
	RenderingSystem::Instance->CreateBuffer(sizeof(uint32) * material_indices.Size(), BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &_StaticModelsMaterialBuffer);

	const void* RESTRICT data_chunks[]{ material_indices.Data() };
	const uint64 data_sizes[]{ sizeof(uint32) * material_indices.Size() };
	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_StaticModelsMaterialBuffer);
}

/*
*	Updates dynamic models.
*/
void RayTracingSystem::UpdateDynamicModels() NOEXCEPT
{
	//Destroy the old data.
	if (_DynamicModelsTopLevelAccelerationStructure)
	{
		RenderingSystem::Instance->DestroyAccelerationStructure(&_DynamicModelsTopLevelAccelerationStructure);
		_DynamicModelsTopLevelAccelerationStructure = EMPTY_HANDLE;
	}

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
	DynamicArray<TopLevelAccelerationStructureInstanceData> instances;
	DynamicArray<uint32> material_indices;

	const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	const DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	uint32 mesh_counter{ 0 };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		for (const uint32 material_index : component->_MaterialIndices)
		{
			material_indices.Emplace(material_index);
		}

		for (const Mesh &mesh : component->_Model->_Meshes)
		{
			instances.Emplace(TopLevelAccelerationStructureInstanceData(component->_CurrentWorldTransform, mesh._BottomLevelAccelerationStructure, RenderingConstants::DYNAMIC_MODELS_HIT_GROUP_INDEX, mesh_counter));

			++mesh_counter;
		}
	}

	//Create the top level acceleration structure.
	RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &_DynamicModelsTopLevelAccelerationStructure);

	//Create the dynamic models material buffer.
	RenderingSystem::Instance->CreateBuffer(sizeof(uint32) * material_indices.Size(), BufferUsage::StorageBuffer, MemoryProperty::DeviceLocal, &_DynamicModelsMaterialBuffer);

	const void* RESTRICT data_chunks[]{ material_indices.Data() };
	const uint64 data_sizes[]{ sizeof(uint32) * material_indices.Size() };
	RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_DynamicModelsMaterialBuffer);
}