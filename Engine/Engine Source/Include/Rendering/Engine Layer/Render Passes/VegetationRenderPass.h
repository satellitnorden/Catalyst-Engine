#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class VegetationRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VegetationRenderPass);

	/*
	*	Initializes the vegetation render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Renders the vegetation entities.
	*/
	void RenderInternal() NOEXCEPT;

};