#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/MobileGraphicsPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) MobileRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(MobileRenderPass);

	/*
	*	Default constructor.
	*/
	MobileRenderPass() NOEXCEPT;

private:

	//The scene depth buffer.
	DepthBufferHandle _SceneDepthBuffer;

	//The mobile graphics pipeline.
	MobileGraphicsPipeline _MobileGraphicsPipeline;

	//The user interface graphics pipeline.
	UserInterfaceGraphicsPipeline _UserInterfaceGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};