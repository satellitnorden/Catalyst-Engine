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
	*	Adds a static instance.
	*/
	void AddStaticInstance(const TopLevelAccelerationStructureInstanceData& instance) NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tabls.
	RenderDataTableHandle _RenderDataTable{ EMPTY_HANDLE };

	//All static instances.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _StaticInstances;

	//Defines if the static top level acceleration structure needs update.
	bool _StaticTopLevelAccelerationStructureNeedsUpdate{ false };

	//The static top level acceleration structure.
	AccelerationStructureHandle _StaticTopLevelAccelerationStructure{ EMPTY_HANDLE };

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

};