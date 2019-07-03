#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DiffuseIrradianceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DiffuseIrradianceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DiffuseIrradianceRenderPass);

	/*
	*	Default constructor.
	*/
	DiffuseIrradianceRenderPass() NOEXCEPT;

private:

	//The diffuse irradiance ray graphics pipeline.
	DiffuseIrradianceGraphicsPipeline _DiffuseIrradianceGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};