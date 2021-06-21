#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/SceneFeaturesDownsampleGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class PostSceneFeaturesRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostSceneFeaturesRenderPass);

	/*
	*	Default constructor.
	*/
	PostSceneFeaturesRenderPass() NOEXCEPT;

private:

	//The scene features downsample graphics pipeline.
	SceneFeaturesDownsampleGraphicsPipeline _SceneFeaturesDownsampleGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};