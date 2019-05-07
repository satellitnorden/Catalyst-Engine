#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/CompositingComputePipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class CompositingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(CompositingRenderPass);

	/*
	*	Default constructor.
	*/
	CompositingRenderPass() NOEXCEPT;

private:

	//The compositing compute pipeline.
	CompositingComputePipeline _CompositingComputePipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};