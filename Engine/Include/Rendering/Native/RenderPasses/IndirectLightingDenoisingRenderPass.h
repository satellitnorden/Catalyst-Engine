#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/IndirectLightingDenoisingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class IndirectLightingDenoisingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(IndirectLightingDenoisingRenderPass);

	/*
	*	Default constructor.
	*/
	IndirectLightingDenoisingRenderPass() NOEXCEPT;

private:

	//The indirect lighting denoising graphics pipelines.
	StaticArray<IndirectLightingDenoisingGraphicsPipeline, 2> _IndirectLightingDenoisingGraphicsPipelines;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};