#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/ShadowsRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ShadowsRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ShadowsRenderPass);

	/*
	*	Default constructor.
	*/
	ShadowsRenderPass() NOEXCEPT;

private:

	//The shadows ray tracing pipeline.
	ShadowsRayTracingPipeline _ShadowsRayTracingPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};