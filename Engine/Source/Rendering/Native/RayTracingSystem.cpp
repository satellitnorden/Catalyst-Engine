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

	//Does the render data table need to be updated?
	if (_TerrainNeedsUpdate || _StaticModelsNeedsUpdate)
	{
		//Rebuild the render data table.
		CreateRenderDataTable();

		//Reset the state.
		_TerrainNeedsUpdate = false;
		_StaticModelsNeedsUpdate = false;
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
	//Create the render data table layout.
	StaticArray<RenderDataTableLayoutBinding, 5> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageBuffer, CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageBuffer, CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(4, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::RayClosestHit),
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

	//Bind the top level acceleration structures to the render data tables.
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(0, 0, &_RenderDataTable, _TerrainTopAccelerationStructure ? _TerrainTopAccelerationStructure : _EmptyTopLevelAccelerationStructure);
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(1, 0, &_RenderDataTable, _StaticModelsTopLevelAccelerationStructure ? _StaticModelsTopLevelAccelerationStructure : _EmptyTopLevelAccelerationStructure);

	//Bind all the static model data.
	{
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

		if (_StaticModelsMaterialUniformBuffer)
		{
			RenderingSystem::Instance->BindUniformBufferToRenderDataTable(4, 0, &_RenderDataTable, _StaticModelsMaterialUniformBuffer);
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
	//Rebuild the static top level acceleration structure.
	{
		if (_StaticModelsTopLevelAccelerationStructure)
		{
			RenderingSystem::Instance->DestroyAccelerationStructure(&_StaticModelsTopLevelAccelerationStructure);
		}

		//Recreate the instances.
		DynamicArray<TopLevelAccelerationStructureInstanceData> instances;

		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		uint32 mesh_counter{ 0 };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			for (const Mesh &mesh : component->_Model->_Meshes)
			{
				instances.Emplace(TopLevelAccelerationStructureInstanceData(component->_WorldTransform, mesh._BottomLevelAccelerationStructure, mesh_counter));

				++mesh_counter;

				ASSERT(mesh_counter < CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, "Increase maximum number of raytraced static meshes!");
			}
		}

		//Create the top level acceleration structure.
		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &_StaticModelsTopLevelAccelerationStructure);
	}

	//Rebuild the static models material uniform buffer.
	{
		if (_StaticModelsMaterialUniformBuffer)
		{
			RenderingSystem::Instance->DestroyBuffer(&_StaticModelsMaterialUniformBuffer);
		}

		RenderingSystem::Instance->CreateBuffer(sizeof(uint32) * CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, BufferUsage::UniformBuffer, MemoryProperty::DeviceLocal, &_StaticModelsMaterialUniformBuffer);

		StaticArray<uint32, CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS> material_indices;

		const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
		const StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

		uint32 mesh_counter{ 0 };

		for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
		{
			for (const uint32 material_index : component->_MaterialIndices)
			{
				material_indices[mesh_counter] = material_index;

				++mesh_counter;

				ASSERT(mesh_counter < CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, "Increase maximum number of raytraced static meshes!");
			}
		}

		const void* RESTRICT data_chunks[]{ material_indices.Data() };
		const uint64 data_sizes[]{ sizeof(uint32) * CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS };
		RenderingSystem::Instance->UploadDataToBuffer(data_chunks, data_sizes, 1, &_StaticModelsMaterialUniformBuffer);
	}
}