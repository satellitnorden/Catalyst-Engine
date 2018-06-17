#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class DirectionalShadowVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalShadowVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalShadowVerticalBlurRenderPass() NOEXCEPT;

private:

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the directional shadow vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional shadow vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};