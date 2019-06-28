#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceSpecularIrradianceGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/SpecularIrradianceApplicationGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class SpecularIrradianceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(SpecularIrradianceRenderPass);

	/*
	*	Default constructor.
	*/
	SpecularIrradianceRenderPass() NOEXCEPT;

private:

	//The screen space specular irradiance graphics pipeline.
	ScreenSpaceSpecularIrradianceGraphicsPipeline _ScreenSpaceSpecularIrradianceGraphicsPipeline;

	//The specular irradiance application graphics pipeline.
	SpecularIrradianceApplicationGraphicsPipeline _SpecularIrradianceApplicationGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};