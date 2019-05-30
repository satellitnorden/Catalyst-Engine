#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DenoisingGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/VolumetricLightingDenoisingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DenoisingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DenoisingRenderPass);

	/*
	*	Default constructor.
	*/
	DenoisingRenderPass() NOEXCEPT;

private:

	//The denoising graphics pipelines.
	StaticArray<DenoisingGraphicsPipeline, 8> _DenoisingGraphicsPipelines;

	//Thevolumetric lighting denoising graphics pipelines.
	StaticArray<VolumetricLightingDenoisingGraphicsPipeline, 8> _VolumetricLightingDenoisingGraphicsPipelines;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};