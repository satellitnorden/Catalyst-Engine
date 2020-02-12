#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/SkyComputePipeline.h>
#include <Rendering/Native/Pipelines/ComputePipelines/SkyDownsampleComputePipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/CloudsApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/CloudsGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/SeparableBlurGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/SkyGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class SkyRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(SkyRenderPass);

	/*
	*	Default constructor.
	*/
	SkyRenderPass() NOEXCEPT;

private:

	//The sky compute pipeline.
	SkyComputePipeline _SkyComputePipeline;

	//The sky downsample compute pipeline.
	SkyDownsampleComputePipeline _SkyDownsampleComputePipeline;

	//The sky graphics pipeline.
	SkyGraphicsPipeline _SkyGraphicsPipeline;

	//The clouds graphics pipeline.
	CloudsGraphicsPipeline _CloudsGraphicsPipeline;

	//The clouds blur pipelines.
	StaticArray<SeparableBlurGraphicsPipeline, 2> _CloudsBlurGraphicsPipelines;

	//The clouds application graphics pipeline.
	CloudsApplicationGraphicsPipeline _CloudsApplicationGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};