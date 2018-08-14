#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class PostProcessingBloomRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingBloomRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingBloomRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle renderDataTableLayout;

	//The render data table.
	RenderDataTableHandle renderDataTable;

	/*
	*	Initializes the post processing bloom render pass.
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
	*	Renders the post processing bloom.
	*/
	void RenderInternal() NOEXCEPT;

};