#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class PostProcessingFogVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingFogVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingFogVerticalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the post processing fog vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post processing fog vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};