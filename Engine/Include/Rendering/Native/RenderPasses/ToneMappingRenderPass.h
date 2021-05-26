#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ToneMappingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ToneMappingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ToneMappingRenderPass);

	/*
	*	Default constructor.
	*/
	ToneMappingRenderPass() NOEXCEPT;

private:

	//The tone mapping graphics pipeline.
	ToneMappingGraphicsPipeline _ToneMappingGraphicsPipeline;

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