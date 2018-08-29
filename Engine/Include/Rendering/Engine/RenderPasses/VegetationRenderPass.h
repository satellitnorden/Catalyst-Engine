#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

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