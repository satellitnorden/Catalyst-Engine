#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class PostProcessingHorizontalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingHorizontalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingHorizontalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the post processing horizontal render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post processing horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};