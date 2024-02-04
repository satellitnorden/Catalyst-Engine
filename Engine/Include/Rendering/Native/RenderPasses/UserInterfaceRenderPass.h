#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/Pipelines/GraphicsPipelines/UserInterfaceGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ALIGN(8) UserInterfaceRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(UserInterfaceRenderPass);

	/*
	*	Default constructor.
	*/
	UserInterfaceRenderPass() NOEXCEPT;

private:

	//The user interface graphics pipeline.
	UserInterfaceGraphicsPipeline _UserInterfaceGraphicsPipeline;

	//The user interface text render pipeline.
	GraphicsRenderPipeline _UserInterfaceTextRenderPipeline{ HashString("UserInterfaceText_RenderPipeline") };

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