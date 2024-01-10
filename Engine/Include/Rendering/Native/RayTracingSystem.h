#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>
#include <Rendering/Native/RayTracing/RayTracingHitGroup.h>

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
	void RenderUpdate(class CommandBuffer *const RESTRICT command_buffer) NOEXCEPT;

	/*
	*	Returns the render data table layout.
	*/
	FORCE_INLINE NO_DISCARD RenderDataTableLayoutHandle GetRenderDataTableLayout() NOEXCEPT
	{
		return _RenderDataTableLayout;
	}

	/*
	*	Returns the current render data table.
	*/
	NO_DISCARD RenderDataTableHandle GetCurrentRenderDataTable() NOEXCEPT;

	/*
	*	Callback for when an entity is initialized.
	*/
	void OnEntityInitialized(const Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Callback for when an entity is terminated.
	*/
	void OnEntityTerminated(const Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Returns if ray tracing data exists.
	*/
	FORCE_INLINE NO_DISCARD bool DoesRayTracingDataExist() const NOEXCEPT
	{
		return !_TopLevelAccelerationStructureInstanceData.Empty();
	}

private:

	//The hit groups.
	DynamicArray<RayTracingHitGroup> _HitGroups;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//Container for all the top level acceleration structure instance data.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _TopLevelAccelerationStructureInstanceData;

	//The top level acceleration structures.
	DynamicArray<AccelerationStructureHandle> _TopLevelAccelerationStructures;

	/*
	*	Returns the hit group with the given identifier.
	*/
	NO_DISCARD RayTracingHitGroup *const RESTRICT GetHitGroup(const HashString identifier) NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};