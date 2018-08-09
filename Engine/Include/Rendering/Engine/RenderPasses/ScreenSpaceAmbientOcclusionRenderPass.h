#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class ScreenSpaceAmbientOcclusionRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenSpaceAmbientOcclusionRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenSpaceAmbientOcclusionRenderPass() NOEXCEPT;

private:

	//The noise scale.
	Vector2 noiseScale;

	/*
	*	Initializes the screen space ambient occlusion render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the screen space ambient occlusion.
	*/
	void RenderInternal() NOEXCEPT;

};