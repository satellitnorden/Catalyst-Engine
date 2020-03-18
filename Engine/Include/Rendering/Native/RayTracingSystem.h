#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>

class RayTracingSystem final
{

public:

	/*
	*	Post-initializes the ray tracing system.
	*/
	void PostInitialize();

	/*
	*	Updates the ray tracing system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the render data table layout.
	*/
	FORCE_INLINE RenderDataTableLayoutHandle GetRenderDataTableLayout() NOEXCEPT
	{
		return _RenderDataTableLayout;
	}

	/*
	*	Returns the render data table.
	*/
	FORCE_INLINE RenderDataTableHandle GetRenderDataTable() NOEXCEPT
	{
		return _RenderDataTable;
	}

	/*
	*	Sets the terrain top level acceleration structure.
	*/
	void SetTerrainTopLevelAccelerationStructure(const AccelerationStructureHandle handle) NOEXCEPT;

	/*
	*	Notifies the ray tracing system that a static model was initialized.
	*/
	void NofityStaticModelInitialized() NOEXCEPT;

	/*
	*	Notifies the ray tracing system that a static model was terminated.
	*/
	void NofityStaticModelTerminated() NOEXCEPT;

	/*
	*	Notifies the ray tracing system that a dynamic model was initialized.
	*/
	void NofityDynamicModelInitialized() NOEXCEPT;

	/*
	*	Notifies the ray tracing system that a dynamic model was modified.
	*/
	void NofityDynamicModelModified() NOEXCEPT;

	/*
	*	Notifies the ray tracing system that a dynamic model was terminated.
	*/
	void NofityDynamicModelTerminated() NOEXCEPT;

private:

	//The empty top level acceleration structure.
	AccelerationStructureHandle _EmptyTopLevelAccelerationStructure;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tabls.
	RenderDataTableHandle _RenderDataTable{ EMPTY_HANDLE };

	//Defines if the terrain needs update.
	bool _TerrainNeedsUpdate{ false };

	//The terrain top level acceleration structure.
	AccelerationStructureHandle _TerrainTopAccelerationStructure{ EMPTY_HANDLE };

	//Defines if the static models top level acceleration structure needs update.
	bool _StaticModelsNeedsUpdate{ false };

	//The static models top level acceleration structure.
	AccelerationStructureHandle _StaticModelsTopLevelAccelerationStructure{ EMPTY_HANDLE };

	//The static models material buffer.
	BufferHandle _StaticModelsMaterialBuffer{ EMPTY_HANDLE };

	//Defines if the dynamic models top level acceleration structure needs update.
	bool _DynamicModelsNeedsUpdate{ false };

	//The dynamic models top level acceleration structure.
	AccelerationStructureHandle _DynamicModelsTopLevelAccelerationStructure{ EMPTY_HANDLE };

	//The dynamic models material buffer.
	BufferHandle _DynamicModelsMaterialBuffer{ EMPTY_HANDLE };

	/*
	*	Creates the empty top level acceleration structure.
	*/
	void CreateEmptyTopLevelAccelerationStructure() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Updates terrain.
	*/
	void UpdateTerrain() NOEXCEPT;

	/*
	*	Updates static models.
	*/
	void UpdateStaticModels() NOEXCEPT;

	/*
	*	Updates dynamic models.
	*/
	void UpdateDynamicModels() NOEXCEPT;

};