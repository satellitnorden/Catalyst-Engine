#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Lighting.
#include <Lighting/DirectionalLight.h>
#include <Lighting/LightingCore.h>
//Rendering.
#include <Rendering/Native/RenderingCore.h>

class LightingSystem final
{

public:

	/*
	*	Default constructor.
	*/
	LightingSystem() NOEXCEPT
	{
	
	}

	/*
	*	Post-initializes the lighting system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the lighting system.
	*/
	void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the directional light.
	*/
	RESTRICTED DirectionalLight *const RESTRICT GetDirectionalLight() NOEXCEPT
	{
		return &_DirectionalLight;
	}

	/*
	*	Returns the lighting data compute render data table layout.
	*/
	RenderDataTableLayoutHandle GetLightingDataComputeRenderDataTableLayout() const NOEXCEPT
	{
		return _ComputeRenderDataTableLayout;
	}

	/*
	*	Returns the lighting data fragment render data table layout.
	*/
	RenderDataTableLayoutHandle GetLightingDataFragmentRenderDataTableLayout() const NOEXCEPT
	{
		return _FragmentRenderDataTableLayout;
	}

	/*
	*	Returns the current lighting data compute render data table.
	*/
	RenderDataTableHandle GetCurrentLightingDataComputeRenderDataTable() const NOEXCEPT;

	/*
	*	Returns the current lighting data fragment render data table.
	*/
	RenderDataTableHandle GetCurrentLightingDataFragmentRenderDataTable() const NOEXCEPT;

	/*
	*	Returns the directional light visibility render target.
	*/
	RenderTargetHandle GetDirectionalLightVisibilityRenderTarget() const NOEXCEPT
	{
		return _DirectionalLightVisibilityRenderTarget;
	}

	/*
	*	Returns the light visibility render targets.
	*/
	const StaticArray<RenderTargetHandle, LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS>& GetLightVisibilityRenderTargets() const NOEXCEPT
	{
		return _LightVisibilityRenderTargets;
	}

private:

	//The directional light.
	DirectionalLight _DirectionalLight;

	//The compute render data table layout.
	RenderDataTableLayoutHandle _ComputeRenderDataTableLayout;

	//The fragment render data table layout.
	RenderDataTableLayoutHandle _FragmentRenderDataTableLayout;

	//The compute render data tables.
	DynamicArray<RenderDataTableHandle> _ComputeRenderDataTables;

	//The fragment render data tables.
	DynamicArray<RenderDataTableHandle> _FragmentRenderDataTables;

	//The uniform buffers.
	DynamicArray<BufferHandle> _UniformBuffers;

	//The directional light visibility render target.
	RenderTargetHandle _DirectionalLightVisibilityRenderTarget;

	//The light visibility render targets.
	StaticArray<RenderTargetHandle, LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS> _LightVisibilityRenderTargets;

	/*
	*	Creates the render data table layouts.
	*/
	void CreateRenderDataTableLayouts() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

	/*
	*	Creates the uniform buffers.
	*/
	void CreateUniformBuffers() NOEXCEPT;

	/*
	*	Creates the render targets.
	*/
	void CreateRenderTargets() NOEXCEPT;

};