#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class DirectionalStaticPhysicalShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalStaticPhysicalShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalStaticPhysicalShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional static physical shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional static physical entity shadows.
	*/
	void RenderInternal() NOEXCEPT;

};