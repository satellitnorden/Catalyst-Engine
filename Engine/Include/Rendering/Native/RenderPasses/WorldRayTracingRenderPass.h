#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/WorldRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class WorldRayTracingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldRayTracingRenderPass);

	/*
	*	Default constructor.
	*/
	WorldRayTracingRenderPass() NOEXCEPT;

private:

	//The world ray tracing pipeline.
	WorldRayTracingPipeline _WorldRayTracingPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};