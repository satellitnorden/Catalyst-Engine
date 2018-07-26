#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class DirectionalShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional shadow.
	*/
	void RenderInternal() NOEXCEPT;

};