#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>
#include <Rendering/Native/RayTracing/RayTracingHitGroup.h>

//Forward declarations.
class StaticModelInstanceData;

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
	*	Returns the hit groups.
	*/
	FORCE_INLINE NO_DISCARD const DynamicArray<RayTracingHitGroup> &GetHitGroups() const NOEXCEPT
	{
		return _HitGroups;
	}

	/*
	*	Returns the current render data table.
	*/
	NO_DISCARD RenderDataTableHandle GetCurrentRenderDataTable() NOEXCEPT;

	/*
	*	Callback for when a static model instance is created.
	*/
	void OnStaticModelInstanceCreated(Entity *const RESTRICT entity, const StaticModelInstanceData &instance_data) NOEXCEPT;

	/*
	*	Callback for when a static model instance is destroyed.
	*/
	void OnStaticModelInstanceDestroyed(Entity *const RESTRICT entity, const StaticModelInstanceData &instance_data) NOEXCEPT;

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