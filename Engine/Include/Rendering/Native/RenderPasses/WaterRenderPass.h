#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) WaterRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WaterRenderPass);

	/*
	*	Default constructor.
	*/
	WaterRenderPass() NOEXCEPT;

private:

	//The scene features 1 render target.
	RenderTargetHandle _SceneFeatures1RenderTarget;

	//The scene features 2 render target.
	RenderTargetHandle _SceneFeatures2RenderTarget;

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