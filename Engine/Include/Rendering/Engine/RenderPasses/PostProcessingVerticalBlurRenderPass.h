#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class PostProcessingVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingVerticalBlurRenderPass() NOEXCEPT;

private:

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the post processing vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the post processing vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};