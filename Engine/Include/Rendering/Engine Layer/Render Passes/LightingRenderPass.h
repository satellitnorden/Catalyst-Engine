#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class LightingRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LightingRenderPass);

	/*
	*	Default constructor.
	*/
	LightingRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the lighting render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the lighting.
	*/
	void RenderInternal() NOEXCEPT;

};