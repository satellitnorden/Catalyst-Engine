#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class PostProcessingRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the post processing render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post processing.
	*/
	void RenderInternal() NOEXCEPT;

};