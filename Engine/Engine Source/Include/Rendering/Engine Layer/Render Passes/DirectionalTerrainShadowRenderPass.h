#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class DirectionalTerrainShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalTerrainShadowRenderPass);

	/*
	*	Initializes the directional shadow render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the directional terrain shadow.
	*/
	void Render() NOEXCEPT;

};