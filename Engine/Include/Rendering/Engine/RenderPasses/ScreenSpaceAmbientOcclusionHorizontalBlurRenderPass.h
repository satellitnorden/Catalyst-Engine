#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class ScreenSpaceAmbientOcclusionHorizontalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenSpaceAmbientOcclusionHorizontalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenSpaceAmbientOcclusionHorizontalBlurRenderPass() NOEXCEPT;

private:

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the screen space ambient occlusion horizontal blur render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the screen space ambient occlusion horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};