#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class ModelSystem final
{

public:

	/*
	*	Post-initializes the model system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the model system.
	*/
	void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the model data render data table layout.
	*/
	RenderDataTableLayoutHandle GetModelDataRenderDataTableLayout() const NOEXCEPT
	{
		return _ModelDataRenderDataTableLayout;
	}

	/*
	*	Returns the current model data render data table.
	*/
	RenderDataTableHandle GetCurrentModelDataRenderDataTable() const NOEXCEPT;


private:

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