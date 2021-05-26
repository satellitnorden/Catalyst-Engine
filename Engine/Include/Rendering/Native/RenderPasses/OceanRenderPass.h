#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/OceanSceneFeaturesGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/PassthroughGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class OceanRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(OceanRenderPass);

	/*
	*	Default constructor.
	*/
	OceanRenderPass() NOEXCEPT;

private:

	//The scene features 1 copy graphics pipeline.
	PassthroughGraphicsPipeline _SceneFeatures1CopyGraphicsPipeline;

	//The scene features 2 copy graphics pipeline.
	PassthroughGraphicsPipeline _SceneFeatures2CopyGraphicsPipeline;

	//The ocean scene features graphics pipeline.
	OceanSceneFeaturesGraphicsPipeline _OceanSceneFeaturesGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif