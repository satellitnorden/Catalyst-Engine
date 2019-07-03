#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/LuminanceLightingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class LuminanceLightingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(LuminanceLightingRenderPass);

	/*
	*	Default constructor.
	*/
	LuminanceLightingRenderPass() NOEXCEPT;

private:

	//The luminance lighting graphics pipeline.
	LuminanceLightingGraphicsPipeline _LuminanceLightingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};