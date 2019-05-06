#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Lighting.
#include <Lighting/LightingCore.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/VisibilityDenoisingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DirectLightingDenoisingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectLightingDenoisingRenderPass);

	/*
	*	Default constructor.
	*/
	DirectLightingDenoisingRenderPass() NOEXCEPT;

private:

	//The visibility denoising graphics pipelines.
	StaticArray<VisibilityDenoisingGraphicsPipeline, (LightingConstants::MAXIMUM_NUMBER_OF_LIGHTS + 1) * 2> _VisibilityDenoisingGraphicsPipelines;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};