#if defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/SimplifiedModelGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/SimplifiedSkyGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class SimplifiedRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(SimplifiedRenderPass);

	/*
	*	Default constructor.
	*/
	SimplifiedRenderPass() NOEXCEPT;

private:

	//The scene depth buffer.
	DepthBufferHandle _SceneDepthBuffer;

	//The simplified model graphics pipeline.
	SimplifiedModelGraphicsPipeline _SimplifiedModelGraphicsPipeline;

	//The simplified sky graphics pipeline.
	SimplifiedSkyGraphicsPipeline _SimplifiedSkyGraphicsPipeline;

	//The user interface graphics pipeline.
	UserInterfaceGraphicsPipeline _UserInterfaceGraphicsPipeline;

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