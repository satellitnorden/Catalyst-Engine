#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

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

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	/*
	*	Initializes the screen space ambient occlusion horizontal blur render pass.
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
	*	Renders the screen space ambient occlusion horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};