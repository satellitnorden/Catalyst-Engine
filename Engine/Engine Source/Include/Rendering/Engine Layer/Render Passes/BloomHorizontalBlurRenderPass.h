#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

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

	/*
	*	Initializes the bloom horizontal render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the bloom horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};