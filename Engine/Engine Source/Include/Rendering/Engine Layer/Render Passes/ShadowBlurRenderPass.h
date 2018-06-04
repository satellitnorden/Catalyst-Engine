#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class ShadowBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ShadowBlurRenderPass);

	/*
	*	Default constructor.
	*/
	ShadowBlurRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the shadow blur render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the shadow blur.
	*/
	void RenderInternal() NOEXCEPT;

};