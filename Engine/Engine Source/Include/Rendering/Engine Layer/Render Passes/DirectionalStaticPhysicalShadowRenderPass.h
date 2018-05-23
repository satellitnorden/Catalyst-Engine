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
	*	Initializes the directional static physical shadow render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the directional static physical entity shadows.
	*/
	void Render() NOEXCEPT;

};