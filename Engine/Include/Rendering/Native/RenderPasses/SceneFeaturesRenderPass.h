#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/SceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class SceneFeaturesRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(SceneFeaturesRenderPass);

	/*
	*	Default constructor.
	*/
	SceneFeaturesRenderPass() NOEXCEPT;

private:

	//The scene features graphics pipeline.
	SceneFeaturesGraphicsPipeline _SceneFeaturesGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};