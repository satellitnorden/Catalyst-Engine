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
	*	Initializes the lighting render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the lighting.
	*/
	void Render() NOEXCEPT;

};