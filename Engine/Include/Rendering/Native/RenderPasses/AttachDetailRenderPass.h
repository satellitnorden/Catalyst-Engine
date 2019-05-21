#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/AttachDetailGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class AttachDetailRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(AttachDetailRenderPass);

	/*
	*	Default constructor.
	*/
	AttachDetailRenderPass() NOEXCEPT;

private:

	//The attach detail graphics pipeline.
	AttachDetailGraphicsPipeline _AttachDetailGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};