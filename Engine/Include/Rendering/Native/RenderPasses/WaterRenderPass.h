#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class WaterRenderPass final : public RenderPass
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		WaterRenderPass
	);

	/*
	*	Default constructor.
	*/
	WaterRenderPass() NOEXCEPT;

private:

	//The scene render target.
	RenderTargetHandle _SceneRenderTarget;

	//The water render pipeline.
	GraphicsRenderPipeline _WaterRenderPipeline{ HashString("Water_RenderPipeline") };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Pre-record.
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