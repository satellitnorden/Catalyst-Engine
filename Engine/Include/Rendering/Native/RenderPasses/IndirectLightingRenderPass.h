#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class IndirectLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(IndirectLightingRenderPass);

	/*
	*	Default constructor.
	*/
	IndirectLightingRenderPass() NOEXCEPT;

private:

	//The indirect lighting graphics pipeline.
	IndirectLightingGraphicsPipeline _IndirectLightingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};