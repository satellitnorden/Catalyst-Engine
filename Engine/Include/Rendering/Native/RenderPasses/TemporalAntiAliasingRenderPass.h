#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAntiAliasingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class TemporalAntiAliasingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(TemporalAntiAliasingRenderPass);

	/*
	*	Default constructor.
	*/
	TemporalAntiAliasingRenderPass() NOEXCEPT;

private:

	//The temporal anti aliasing graphics pipelines.
	StaticArray<TemporalAntiAliasingGraphicsPipeline, RenderingConstants::TEMPORAL_ANTI_ALIASING_SAMPLES> _TemporalAntiAliasingGraphicsPipelines;

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};