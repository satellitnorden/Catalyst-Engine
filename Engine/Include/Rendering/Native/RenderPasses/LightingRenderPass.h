#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/ComputePipelines/LightingComputePipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class LightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(LightingRenderPass);

	/*
	*	Default constructor.
	*/
	LightingRenderPass() NOEXCEPT;

private:

	//The lighting compute pipeline.
	LightingComputePipeline _LightingComputePipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};