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
	StaticArray<RenderDataTableLayoutBinding, 4> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(1, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayClosestHit | ShaderStage::RayGeneration),
		RenderDataTableLayoutBinding(2, RenderDataTableLayoutBinding::Type::StorageBuffer, CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, ShaderStage::RayClosestHit),
		RenderDataTableLayoutBinding(3, RenderDataTableLayoutBinding::Type::StorageBuffer, CatalystShaderConstants::MAXIMUM_NUMBER_OF_RAY_TRACED_STATIC_MODELS, ShaderStage::RayClosestHit)
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

	//Bind all the static model vertex/index buffers.
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
		}
	}

	//Create the top level acceleration structure.
	RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(instances), &_StaticModelsTopLevelAccelerationStructure);
}