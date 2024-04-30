#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugRenderSphereGraphicsPipeline.h>
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

#if defined(CATALYST_EDITOR)
	//The editor graphics render pipelines.
	StaticArray<GraphicsRenderPipeline, 2> _EditorGraphicsRenderPipelines
	{
		GraphicsRenderPipeline(HashString("PlayerSpawnMarker_RenderPipeline")),
		GraphicsRenderPipeline(HashString("CircleMarker_RenderPipeline"))
	};
#endif

	//The debug render axis aligned bounding box 3D graphics pipelines.
	StaticArray<DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline, 4> _DebugRenderAxisAlignedBoundingBox3DGraphicsPipelines;

	//The debug render sphere graphics pipeline
	DebugRenderSphereGraphicsPipeline _DebugRenderSphereGraphicsPipeline;

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