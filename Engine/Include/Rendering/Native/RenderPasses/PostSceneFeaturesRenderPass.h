#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class PostSceneFeaturesRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		PostSceneFeaturesRenderPass
	);

	/*
	*	Default constructor.
	*/
	PostSceneFeaturesRenderPass() NOEXCEPT;

private:

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//The only lighting pipeline.
	GraphicsRenderPipeline _OnlyLightingPipeline{ HashString("OnlyLighting_RenderPipeline") };
#endif

	//The scene features downsample graphics pipeline.
	GraphicsRenderPipeline _SceneFeaturesDownsampleGraphicsPipeline{ HashString("SceneFeaturesDownsample_RenderPipeline") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};