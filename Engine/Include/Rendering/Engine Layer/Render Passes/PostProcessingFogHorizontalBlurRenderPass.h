#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class PostProcessingFogHorizontalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingFogHorizontalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingFogHorizontalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the post processing fog horizontal render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post processing fog horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};
