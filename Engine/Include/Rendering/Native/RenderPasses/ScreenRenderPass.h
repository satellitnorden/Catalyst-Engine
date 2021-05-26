#if !defined(CATALYST_SIMPLIFIED_RENDERING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/ScreenGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class ScreenRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(ScreenRenderPass);

	/*
	*	Default constructor.
	*/
	ScreenRenderPass() NOEXCEPT;

private:

	//The Screen graphics pipeline.
	ScreenGraphicsPipeline _ScreenGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};
#endif