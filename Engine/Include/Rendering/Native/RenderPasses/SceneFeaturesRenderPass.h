#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/AnimatedModelSceneFeaturesGraphicsPipeline.h>
#if defined(CATALYST_EDITOR)
#include <Rendering/Native/Pipelines/GraphicsPipelines/EditorSelectedModelGraphicsPipeline.h>
#endif
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) SceneFeaturesRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(SceneFeaturesRenderPass);

	/*
	*	Default constructor.
	*/
	SceneFeaturesRenderPass() NOEXCEPT;

private:

	//The graphics render pipelines.
	StaticArray<GraphicsRenderPipeline, 15> _GraphicsRenderPipelines
	{
		//GraphicsRenderPipeline(HashString("Clear_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelDepthSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelDepthDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelDepthSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelDepthDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedImpostorDepth_RenderPipeline")),
		GraphicsRenderPipeline(HashString("Grass_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("OpaqueModelDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelColorSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("MaskedModelColorDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelColorSingleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedModelColorDoubleSided_RenderPipeline")),
		GraphicsRenderPipeline(HashString("InstancedImpostorColor_RenderPipeline")),
		GraphicsRenderPipeline(HashString("Terrain_RenderPipeline")),
		GraphicsRenderPipeline(HashString("Particles_RenderPipeline"))
	};

	//The animated model scene features graphics pipeline.
	AnimatedModelSceneFeaturesGraphicsPipeline _AnimatedModelSceneFeaturesGraphicsPipeline;

#if defined(CATALYST_EDITOR)
	//The editor selected model graphics pipeline.
	EditorSelectedModelGraphicsPipeline _EditorSelectedModelGraphicsPipeline;
#endif

	//The velocity graphics pipeline.
	GraphicsRenderPipeline _VelocityGraphicsPipeline{ HashString("Velocity_RenderPipeline") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Pre records this render pass.
	*/
	void PreRecord(CommandBuffer *const RESTRICT command_buffer) NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};