#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/TemporalAccumulationComputePipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class TemporalAccumulationRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(TemporalAccumulationRenderPass);

	/*
	*	Default constructor.
	*/
	TemporalAccumulationRenderPass() NOEXCEPT;

private:

	//The temporal accumulation compute pipeline.
	TemporalAccumulationComputePipeline _TemporalAccumulationComputePipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};