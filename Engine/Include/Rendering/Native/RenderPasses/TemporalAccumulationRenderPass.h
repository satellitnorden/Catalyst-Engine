#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAccumulationGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class TemporalAccumulationRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(TemporalAccumulationRenderPass);

	/*
	*	Default constructor.
	*/
	TemporalAccumulationRenderPass() NOEXCEPT;

private:

	//The temporal diffuse irradiance accumulation graphics pipelines.
	StaticArray<TemporalAccumulationGraphicsPipeline, 2> _DiffuseIrradianceTemporalAccumulationGraphicsPipelines;

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