#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class BloomUpsampleSecondIterationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomUpsampleSecondIterationRenderPass);

	/*
	*	Default constructor.
	*/
	BloomUpsampleSecondIterationRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes the bloom upsample second iteration render pass.
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
	*	Renders the bloom upsample second iteration.
	*/
	void RenderInternal() NOEXCEPT;

};