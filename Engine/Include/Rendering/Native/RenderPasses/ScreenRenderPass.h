#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/GraphicsRenderPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

//Systems.
#include <Systems/System.h>

class ScreenRenderPass final : public RenderPass
{
	
public:

	//System declaration.
	CATALYST_SYSTEM
	(
		ScreenRenderPass
	);

	/*
	*	Default constructor.
	*/
	ScreenRenderPass() NOEXCEPT;

private:

	//The Screen graphics pipeline.
	GraphicsRenderPipeline _ScreenGraphicsPipeline{ HashString("Screen_RenderPipeline") };

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