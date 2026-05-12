#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class SceneFeaturesRenderPass final : public RenderPass
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		SceneFeaturesRenderPass
	);

	/*
	*	Default constructor.
	*/
	SceneFeaturesRenderPass() NOEXCEPT;

private:

	//The graphics render pipelines.
	StaticArray<GraphicsRenderPipeline, 17> _GraphicsRenderPipelines
	{
		GraphicsRenderPipeline(HashString("MaskedModelDepthSingleSided")),
		GraphicsRenderPipeline(HashString("MaskedModelDepthDoubleSided")),
		GraphicsRenderPipeline(HashString("InstancedModelDepthSingleSided")),
		GraphicsRenderPipeline(HashString("InstancedModelDepthDoubleSided")),
		GraphicsRenderPipeline(HashString("InstancedImpostorDepth")),
		GraphicsRenderPipeline(HashString("Grass")),
		GraphicsRenderPipeline(HashString("OpaqueModelSingleSided")),
		GraphicsRenderPipeline(HashString("OpaqueModelDoubleSided")),
		GraphicsRenderPipeline(HashString("MaskedModelColorSingleSided")),
		GraphicsRenderPipeline(HashString("MaskedModelColorDoubleSided")),
		GraphicsRenderPipeline(HashString("InstancedModelColorSingleSided")),
		GraphicsRenderPipeline(HashString("InstancedModelColorDoubleSided")),
		GraphicsRenderPipeline(HashString("InstancedImpostorColor")),
		GraphicsRenderPipeline(HashString("Terrain")),
		GraphicsRenderPipeline(HashString("Particles")),
		GraphicsRenderPipeline(HashString("AnimatedModel")),
		GraphicsRenderPipeline(HashString("UserInterface3DBackground"))
	};

#if 0
	//The user interface 3D pipeline.
	GraphicsRenderPipeline _UserInterface3DPipeline{ HashString("UserInterface") };
#endif

	//The velocity graphics pipeline.
	GraphicsRenderPipeline _VelocityGraphicsPipeline{ HashString("Velocity") };

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