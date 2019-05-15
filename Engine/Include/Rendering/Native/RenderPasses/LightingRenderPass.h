#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/LightingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

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

	//The lighting graphics pipeline.
	LightingGraphicsPipeline _LightingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};