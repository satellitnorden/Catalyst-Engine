#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/ShaderLightComponent.h>

class LightingSystem final
{

public:

	/*
	*	Post-initializes the lighting system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the lighting system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the lighting data render data table layout.
	*/
	RenderDataTableLayoutHandle GetLightingDataRenderDataTableLayout() const NOEXCEPT
	{
		return _RenderDataTableLayout;
	}

	/*
	*	Returns the current lighting data render data table.
	*/
	RenderDataTableHandle GetCurrentLightingDataRenderDataTable() const NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The light uniform data buffers.
	DynamicArray<BufferHandle> _LightUniformDataBuffers;

	//The light data buffers.
	DynamicArray<BufferHandle> _LightDataBuffers;

	//The light data buffer sizes.
	DynamicArray<uint64> _LightDataBufferSizes;

	//The shader light components.
	DynamicArray<ShaderLightComponent> _ShaderLightComponents;

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
	void CreateLightUniformDataBuffers() NOEXCEPT;

	/*
	*	Creates the light data buffers.
	*/
	void CreateLightDataBuffers() NOEXCEPT;

};