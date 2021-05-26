#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugRenderAxisAlignedBoundingBox3DGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugRenderSphereGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DebugRenderingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DebugRenderingRenderPass);

	/*
	*	Default constructor.
	*/
	DebugRenderingRenderPass() NOEXCEPT;

private:

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

};
#endif
#endif