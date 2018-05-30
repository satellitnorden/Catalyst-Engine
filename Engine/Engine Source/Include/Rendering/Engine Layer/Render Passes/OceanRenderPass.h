#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class OceanRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(OceanRenderPass);

	/*
	*	Initializes the ocean render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the ocean.
	*/
	void RenderInternal() NOEXCEPT;

};