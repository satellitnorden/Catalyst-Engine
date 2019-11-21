//Header file.
#include <Rendering/Native/RayTracingSystem.h>

//Systems.
#include <Systems/RenderingSystem.h>

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
void RayTracingSystem::RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT
{
	//No need to do anything if ray tracing isn't active.
	if (!RenderingSystem::Instance->IsRayTracingActive())
	{
		return;
	}

	if (_StaticTopLevelAccelerationStructureNeedsUpdate)
	{
		//Rebuild the static top level acceleration structure.
		if (_StaticTopLevelAccelerationStructure)
		{
			RenderingSystem::Instance->DestroyTopLevelAccelerationStructure(&_StaticTopLevelAccelerationStructure);
		}

		RenderingSystem::Instance->CreateTopLevelAccelerationStructure(ArrayProxy<TopLevelAccelerationStructureInstanceData>(_StaticInstances), &_StaticTopLevelAccelerationStructure);

		//Rebuild the render data table.
		CreateRenderDataTable();

		_StaticTopLevelAccelerationStructureNeedsUpdate = false;
	}
}

/*
*	Adds a static instance.
*/
void RayTracingSystem::AddStaticInstance(const TopLevelAccelerationStructureInstanceData& instance) NOEXCEPT
{
	//Add the static instance.
	_StaticInstances.EmplaceSlow(instance);

	//Flag that the static top level acceleration structure needs update.
	_StaticTopLevelAccelerationStructureNeedsUpdate = true;
}

/*
*	Creates the render data table layout.
*/
void RayTracingSystem::CreateRenderDataTableLayout() NOEXCEPT
{
	//Create the render data table layout.
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::AccelerationStructure, 1, ShaderStage::RayGeneration)
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

	//Bind the static top level acceleration structure to it.
	RenderingSystem::Instance->BindAccelerationStructureToRenderDataTable(0, 0, &_RenderDataTable, _StaticTopLevelAccelerationStructure);
}