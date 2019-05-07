#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AntiAliasingGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class AntiAliasingRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(AntiAliasingRenderPass);

	/*
	*	Default constructor.
	*/
	AntiAliasingRenderPass() NOEXCEPT;

private:

	//The anti-aliasing graphics pipeline.
	AntiAliasingGraphicsPipeline _AntiAliasingGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};