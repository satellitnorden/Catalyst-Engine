#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class DirectionalShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalShadowRenderPass);

	/*
	*	Initializes the directional shadow render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the directional shadow.
	*/
	void Render() NOEXCEPT;

};