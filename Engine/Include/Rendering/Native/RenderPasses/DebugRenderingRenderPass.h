#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class DebugRenderingRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		DebugRenderingRenderPass,
		NOT_DEFINED_REQUIREMENT(CATALYST_CONFIGURATION_FINAL)
	);

	/*
	*	Default constructor.
	*/
	DebugRenderingRenderPass() NOEXCEPT;

private:

	//The pipelines.
	StaticArray<GraphicsRenderPipeline, 4> _Pipelines
	{
		GraphicsRenderPipeline(HashString("DebugRenderCircle_RenderPipeline")),
		GraphicsRenderPipeline(HashString("DebugRenderRectangle_RenderPipeline")),
		GraphicsRenderPipeline(HashString("PlayerSpawnMarker_RenderPipeline")),
		GraphicsRenderPipeline(HashString("CircleMarker_RenderPipeline"))
	};

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
#endif