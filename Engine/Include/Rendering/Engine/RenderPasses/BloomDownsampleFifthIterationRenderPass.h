#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class BloomDownsampleFifthIterationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomDownsampleFifthIterationRenderPass);

	/*
	*	Default constructor.
	*/
	BloomDownsampleFifthIterationRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes the bloom downsample fifth iteration render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the bloom downsample fifth iteration.
	*/
	void RenderInternal() NOEXCEPT;

};