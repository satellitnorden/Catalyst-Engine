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
	*	Default constructor.
	*/
	VegetationRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the vegetation render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the vegetation entities.
	*/
	void RenderInternal() NOEXCEPT;

};