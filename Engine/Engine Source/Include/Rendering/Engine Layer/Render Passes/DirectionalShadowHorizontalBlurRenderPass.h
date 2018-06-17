#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class DirectionalShadowHorizontalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalShadowHorizontalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalShadowHorizontalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the directional shadow horizontal render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional shadow horizontal blur.
	*/
	void RenderInternal() NOEXCEPT;

};