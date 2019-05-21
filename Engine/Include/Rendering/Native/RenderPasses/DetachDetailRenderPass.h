#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DetachDetailGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DetachDetailRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(DetachDetailRenderPass);

	/*
	*	Default constructor.
	*/
	DetachDetailRenderPass() NOEXCEPT;

private:

	//The detach detail graphics pipeline.
	DetachDetailGraphicsPipeline _DetachDetailGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};