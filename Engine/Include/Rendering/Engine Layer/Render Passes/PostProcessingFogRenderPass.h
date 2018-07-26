#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class PostProcessingFogRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingFogRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingFogRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the post processing fog render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post fog processing.
	*/
	void RenderInternal() NOEXCEPT;

};