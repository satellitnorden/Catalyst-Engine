#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/OceanGraphicsPipeline.h>
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

	//The ocean graphics pipeline.
	OceanGraphicsPipeline _OceanGraphicsPipeline;

	//The copy graphics pipeline.
	PassthroughGraphicsPipeline _CopyGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};