#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class VerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	VerticalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};