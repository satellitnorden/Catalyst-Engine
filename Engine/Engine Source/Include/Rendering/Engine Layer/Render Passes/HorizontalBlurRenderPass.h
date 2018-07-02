#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class HorizontalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HorizontalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	HorizontalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the horizontal render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};