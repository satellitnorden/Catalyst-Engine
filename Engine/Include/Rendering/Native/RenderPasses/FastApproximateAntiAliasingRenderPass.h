#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/FastApproximateAntiAliasingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class FastApproximateAntiAliasingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(FastApproximateAntiAliasingRenderPass);

	/*
	*	Default constructor.
	*/
	FastApproximateAntiAliasingRenderPass() NOEXCEPT;

private:

	//The fast approximate anti aliasing graphics pipeline.
	FastApproximateAntiAliasingGraphicsPipeline _FastApproximateAntiAliasingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif