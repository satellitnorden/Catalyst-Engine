#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/ComputeRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) ScreenshotAccumulationRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenshotAccumulationRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenshotAccumulationRenderPass() NOEXCEPT;

private:

	//Denotes if we are currently taking a screenshot.
	bool _IsTakingScreenshot;

	//The accumulation buffer.
	RenderTargetHandle _AccumulationBuffer;

	//The number of samples.
	uint32 _NumberOfSamples;

	//The screenshot accumulation pipeline.
	ComputeRenderPipeline _ScreenshotAccumulationPipeline{ HashString("ScreenshotAccumulation_RenderPipeline") };

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