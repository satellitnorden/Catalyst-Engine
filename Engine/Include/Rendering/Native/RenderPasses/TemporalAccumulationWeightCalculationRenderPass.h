#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAccumulationWeightCalculationGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class TemporalAccumulationWeightCalculationRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(TemporalAccumulationWeightCalculationRenderPass);

	/*
	*	Default constructor.
	*/
	TemporalAccumulationWeightCalculationRenderPass() NOEXCEPT;

private:

	//The temporal accumulation weight calculation graphics pipelines.
	StaticArray<TemporalAccumulationWeightCalculationGraphicsPipeline, 2> _TemporalAccumulationWeightCalculationGraphicsPipelines;

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