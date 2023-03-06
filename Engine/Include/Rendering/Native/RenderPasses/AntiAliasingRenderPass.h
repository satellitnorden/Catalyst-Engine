#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/TemporalAntiAliasingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

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

	//The render targets.
	StaticArray<RenderTargetHandle, 2> _RenderTargets;

	//The temporal anti aliasing graphics pipelines.
	StaticArray<TemporalAntiAliasingGraphicsPipeline, 2> _TemporalAntiAliasingGraphicsPipelines;

	//The current buffer index.
	uint8 _CurrentBufferIndex{ 0 };

	//The number of accumulations.
	uint32 _NumberOfAccumulations{ 0 };

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