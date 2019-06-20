#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/BloomApplicationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/BloomIsolationGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ResampleGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class BloomRenderPass final : public RenderPass
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

	//The bloom resample graphics pipelines.
	StaticArray<ResampleGraphicsPipeline, 4> _BloomResampleGraphicsPipelines;

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

};