#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/RenderingConfiguration.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/FastApproximateAntiAliasingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAntiAliasingGraphicsPipeline.h>

class ALIGN(8) AntiAliasingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(AntiAliasingRenderPass);

	/*
	*	Default constructor.
	*/
	AntiAliasingRenderPass() NOEXCEPT;

private:

	/////////////
	// GENERAL //
	/////////////

	//The current anti aliasing mode.
	RenderingConfiguration::AntiAliasingMode _CurrentAntiAliasingMode{ RenderingConfiguration::AntiAliasingMode::NONE };

	////////////////////////////////////
	// FAST APPROXIMATE ANTI ALIASING //
	////////////////////////////////////

	//The fast approximate anti aliasing graphics pipeline.
	FastApproximateAntiAliasingGraphicsPipeline _FastApproximateAntiAliasingGraphicsPipeline;

	////////////////////////////
	// TEMPORAL ANTI ALIASING //
	////////////////////////////

	//The temporal anti aliasing render targets.
	StaticArray<RenderTargetHandle, 2> _TemporalAntiAliasingRenderTargets;

	//The temporal anti aliasing graphics pipelines.
	StaticArray<TemporalAntiAliasingGraphicsPipeline, 2> _TemporalAntiAliasingGraphicsPipelines;

	//The current temporal anti aliasing buffer index.
	uint8 _TemporalAntiAliasingCurrentBufferIndex{ 0 };

	//The number of temporal anti aliasing accumulations.
	uint32 _TemporalAntiAliasingNumberOfAccumulations{ 0 };

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