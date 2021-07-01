#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/ShaderLightComponent.h>

class VirtualRealitySystem final
{

public:

	/*
	*	Post-initializes the virtual reality system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the lighting system during the render update phase.
	*/
	void RenderUpdate() NOEXCEPT;

	/*
	*	Returns the virtual reality data render data table layout.
	*/
	RenderDataTableLayoutHandle GetVirtualRealityDataRenderDataTableLayout() const NOEXCEPT
	{
		return _RenderDataTableLayout;
	}

	/*
	*	Returns the current virtual reality data render data table.
	*/
	RenderDataTableHandle GetCurrentVirtualRealityDataRenderDataTable() const NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The virtual reality uniform data buffers.
	DynamicArray<BufferHandle> _VirtualRealityUniformDataBuffers;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

	/*
	*	Creates the light uniform data buffers.
	*/
	void CreateVirtualRealityUniformDataBuffers() NOEXCEPT;

};