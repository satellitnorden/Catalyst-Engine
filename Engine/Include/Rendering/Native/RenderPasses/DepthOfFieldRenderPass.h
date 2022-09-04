#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldBokehBlurGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DepthOfFieldFloodfillBlurGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) DepthOfFieldRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DepthOfFieldRenderPass);

	/*
	*	Default constructor.
	*/
	DepthOfFieldRenderPass() NOEXCEPT;

private:

	//The depth of field bokeh blur graphics pipeline.
	DepthOfFieldBokehBlurGraphicsPipeline _DepthOfFieldBokehBlurGraphicsPipeline;

	//The depth of field floodfill blur graphics pipelines.
	StaticArray<DepthOfFieldFloodfillBlurGraphicsPipeline, 2> _DepthOfFieldFloodfillBlurGraphicsPipelines;

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

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};