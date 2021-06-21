#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/PostProcessingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class PostProcessingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingRenderPass() NOEXCEPT;

private:

	//The post processing graphics pipeline.
	PostProcessingGraphicsPipeline _PostProcessingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};