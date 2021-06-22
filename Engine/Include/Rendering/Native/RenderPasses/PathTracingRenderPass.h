#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/RayTracingPipelines/PathTracingRayTracingPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class PathTracingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(PathTracingRenderPass);

	/*
	*	Default constructor.
	*/
	PathTracingRenderPass() NOEXCEPT;

private:

	//The path tracing ray tracing pipeline.
	PathTracingRayTracingPipeline _PathTracingRayTracingPipeline;

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