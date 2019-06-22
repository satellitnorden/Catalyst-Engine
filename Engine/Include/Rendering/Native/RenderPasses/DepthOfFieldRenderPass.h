#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ResampleGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/SeparableBlurGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DepthOfFieldRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DepthOfFieldRenderPass);

	/*
	*	Default constructor.
	*/
	DepthOfFieldRenderPass() NOEXCEPT;

private:

	//The depth of field downsample graphics pipelines.
	StaticArray<ResampleGraphicsPipeline, 2> _DepthOfFieldDownsampleGraphicsPipelines;

	//The depth of field separable blur graphics pipelines.
	StaticArray<SeparableBlurGraphicsPipeline, 2> _DepthOfFieldSeparableBlurGraphicsPipelines;

	//The depth of field upsample graphics pipeline.
	ResampleGraphicsPipeline _DepthOfFieldUpsampleGraphicsPipeline;

	//The depth of field application graphics pipeline.
	DepthOfFieldApplicationGraphicsPipeline _DepthOfFieldApplicationGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};