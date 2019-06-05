#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenSpaceSpecularIrradianceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ScreenSpaceSpecularIrradianceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenSpaceSpecularIrradianceRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenSpaceSpecularIrradianceRenderPass() NOEXCEPT;

private:

	//The screen space specular irradiance graphics pipeline.
	ScreenSpaceSpecularIrradianceGraphicsPipeline _ScreenSpaceSpecularIrradianceGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};