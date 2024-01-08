#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

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
	void RenderUpdate() NOEXCEPT;

	/*
	*	Returns the top level acceleration structure.
	*/
	FORCE_INLINE NO_DISCARD AccelerationStructureHandle GetTopLevelAccelerationStructure() const NOEXCEPT
	{
		return _TopLevelAccelerationStructure;
	}

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
	*	Reports that dynamic models have been updated.
	*/
	FORCE_INLINE void ReportDynamicModelsUpdated() NOEXCEPT
	{
		_DynamicModelsUpdated = true;
	}

	/*
	*	Reports that static models have been updated.
	*/
	FORCE_INLINE void ReportStaticModelsUpdated() NOEXCEPT
	{
		_StaticModelsUpdated = true;
	}

	/*
	*	Sets the terrain bottom level acceleration structure.
	*/
	void SetTerrainBottomLevelAccelerationStructure(const AccelerationStructureHandle handle) NOEXCEPT;

	/*
	*	Returns if ray tracing data exists.
	*/
	FORCE_INLINE NO_DISCARD bool DoesRayTracingDataExist() const NOEXCEPT
	{
		return !_TopLevelAccelerationStructureInstanceData.Empty();
	}

private:

	//Container for all the top level acceleration structure instance data.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _TopLevelAccelerationStructureInstanceData;

	//The top level acceleration structure.
	AccelerationStructureHandle _TopLevelAccelerationStructure{ EMPTY_HANDLE };

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tabls.
	RenderDataTableHandle _RenderDataTable{ EMPTY_HANDLE };

	//The terrain bottom level acceleration structure.
	AccelerationStructureHandle _TerrainBottomAccelerationStructure{ EMPTY_HANDLE };

	//Denotes whether or not dynamic models have been updated.
	bool _DynamicModelsUpdated{ false };

	//Container for all the dynamic model top level acceleration structure instance data.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _DynamicModelTopLevelAccelerationStructureInstanceData;

	//The dynamic models material indices.
	DynamicArray<uint32> _DynamicModelsMaterialindices;

	//The dynamic models material buffer.
	BufferHandle _DynamicModelsMaterialBuffer{ EMPTY_HANDLE };

	//Denotes whether or not static models have been updated.
	bool _StaticModelsUpdated{ false };

	//Container for all the static model top level acceleration structure instance data.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _StaticModelTopLevelAccelerationStructureInstanceData;

	//The static models material indices.
	DynamicArray<uint32> _StaticModelsMaterialindices;

	//The static models material buffer.
	BufferHandle _StaticModelsMaterialBuffer{ EMPTY_HANDLE };

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
	*	Updates dynamic models.
	*/
	void UpdateDynamicModels() NOEXCEPT;

	/*
	*	Updates static models.
	*/
	void UpdateStaticModels() NOEXCEPT;

};