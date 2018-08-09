#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class ScreenSpaceAmbientOcclusionVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenSpaceAmbientOcclusionVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenSpaceAmbientOcclusionVerticalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the screen space ambient occlusion vertical blur render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the screen space ambient occlusion vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};