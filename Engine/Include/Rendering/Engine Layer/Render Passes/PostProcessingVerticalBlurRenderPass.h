#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

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

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the post processing vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post processing vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};