#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/BloomApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/BloomIsolationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ResampleGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) BloomRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomRenderPass);

	/*
	*	Default constructor.
	*/
	BloomRenderPass() NOEXCEPT;

private:

	//The bloom isolation graphics pipeline.
	BloomIsolationGraphicsPipeline _BloomIsolationGraphicsPipeline;

	//The bloom downsample graphics pipelines.
	StaticArray<ResampleGraphicsPipeline, 7> _BloomDownsampleGraphicsPipelines;

	//The bloom upsample graphics pipelines.
	StaticArray<ResampleGraphicsPipeline, 7> _BloomUpsampleGraphicsPipelines;

	//The bloom application graphics pipeline.
	BloomApplicationGraphicsPipeline _BloomApplicationGraphicsPipeline;

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