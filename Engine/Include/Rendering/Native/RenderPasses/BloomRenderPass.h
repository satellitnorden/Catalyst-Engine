#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class BloomRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		BloomRenderPass
	);

	/*
	*	Default constructor.
	*/
	BloomRenderPass() NOEXCEPT;

private:

	//The bloom downsample pipelines.
	StaticArray<GraphicsRenderPipeline, 5> _BloomDownsamplePipelines
	{
		GraphicsRenderPipeline(HashString("BloomDownsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomDownsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomDownsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomDownsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomDownsample_RenderPipeline"))
	};

	StaticArray<GraphicsRenderPipeline, 4> _BloomUpsamplePipelines
	{
		GraphicsRenderPipeline(HashString("BloomUpsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomUpsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomUpsample_RenderPipeline")),
		GraphicsRenderPipeline(HashString("BloomUpsample_RenderPipeline"))
	};

	//The bloom application pipeline.
	GraphicsRenderPipeline _BloomApplicationPipeline{ HashString("BloomApplication_RenderPipeline") };

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