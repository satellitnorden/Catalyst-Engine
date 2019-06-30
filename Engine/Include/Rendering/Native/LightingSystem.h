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

	/*
	*	Returns the number of active lights.
	*/
	FORCE_INLINE NO_DISCARD int32 GetNumberOfActiveLights() const NOEXCEPT
	{
		return _NumberOfActiveLights;
	}

private:

	//The directional light.
	DirectionalLight _DirectionalLight;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The uniform buffers.
	DynamicArray<BufferHandle> _UniformBuffers;

	//The number of active lights.
	int32 _NumberOfActiveLights;

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