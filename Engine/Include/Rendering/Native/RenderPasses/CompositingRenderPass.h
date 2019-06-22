#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/CompositingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/ResampleGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class CompositingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(CompositingRenderPass);

	/*
	*	Default constructor.
	*/
	CompositingRenderPass() NOEXCEPT;

private:

	//The diffuse irradiance resample graphics pipeline.
	ResampleGraphicsPipeline _DiffuseIrradianceResampleGraphicsPipeline;

	//The volumetric lighting resample graphics pipeline.
	ResampleGraphicsPipeline _VolumetricLightingResampleGraphicsPipeline;

	//The compositing graphics pipeline.
	CompositingGraphicsPipeline _CompositingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};