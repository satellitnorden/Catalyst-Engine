#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalShadowVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalShadowVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalShadowVerticalBlurRenderPass() NOEXCEPT;

private:

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The gaussian blur data.
	GaussianBlurData _GaussianBlurData;

	/*
	*	Initializes the directional shadow vertical blur render pass.
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
	*	Initializes the gaussian blur data.
	*/
	void InitializeGaussianBlurData() NOEXCEPT;

	/*
	*	Renders the directional shadow vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};