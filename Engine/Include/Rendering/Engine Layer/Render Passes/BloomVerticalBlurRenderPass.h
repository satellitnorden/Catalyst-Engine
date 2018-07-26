#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

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

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the bloom vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the bloom vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};