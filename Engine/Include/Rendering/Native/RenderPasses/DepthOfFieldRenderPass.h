#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldBlurGraphicsPipeline.h>
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

	//The depth of field blur graphics pipelines.
	StaticArray<DepthOfFieldBlurGraphicsPipeline, 2> _DepthOfFieldBlurGraphicsPipelines;

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