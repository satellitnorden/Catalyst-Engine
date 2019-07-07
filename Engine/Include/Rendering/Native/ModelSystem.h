#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>

class ModelSystem final
{

public:

	/*
	*	Post-initializes the model system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the model system during the pre update phase.
	*/
	void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the model system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the top level acceleration structure instances.
	*/
	FORCE_INLINE const DynamicArray<TopLevelAccelerationStructureInstanceData>& GetTopLevelAccelerationStructureInstances() NOEXCEPT
	{
		return _TopLevelAccelerationStructureInstances;
	}

	/*
	*	Returns the top level acceleration structure.
	*/
	FORCE_INLINE AccelerationStructureHandle GetTopLevelAccelerationStructure() NOEXCEPT
	{
		return _TopLevelAccelerationStructure;
	}

	/*
	*	Returns the model data render data table layout.
	*/
	FORCE_INLINE RenderDataTableLayoutHandle GetModelDataRenderDataTableLayout() const NOEXCEPT
	{
		return _ModelDataRenderDataTableLayout;
	}

	/*
	*	Returns the current model data render data table.
	*/
	RenderDataTableHandle GetCurrentModelDataRenderDataTable() const NOEXCEPT;

private:

	//The top level acceleration structure instances.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _TopLevelAccelerationStructureInstances;

	//The top level acceleration structure.
	AccelerationStructureHandle _TopLevelAccelerationStructure{ EMPTY_HANDLE };

	//The model data render data table layout.
	RenderDataTableLayoutHandle _ModelDataRenderDataTableLayout;

	//The model data render data tables.
	DynamicArray<RenderDataTableHandle> _ModelDataRenderDataTables;

	//The materials uniform buffers.
	DynamicArray<BufferHandle> _MaterialsUniformBuffers;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

	/*
	*	Creates the uniform buffers.
	*/
	void CreateUniformBuffers() NOEXCEPT;

};