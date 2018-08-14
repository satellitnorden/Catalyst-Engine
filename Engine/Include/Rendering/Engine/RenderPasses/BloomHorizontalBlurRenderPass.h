#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class BloomHorizontalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomHorizontalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	BloomHorizontalBlurRenderPass() NOEXCEPT;

private:

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the bloom horizontal render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the bloom horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};