#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class BloomVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	BloomVerticalBlurRenderPass() NOEXCEPT;

private:

	//The render data table.
	RenderDataTableHandle renderDataTable;

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the bloom vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the bloom vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};