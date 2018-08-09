#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class PostProcessingBloomRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PostProcessingBloomRenderPass);

	/*
	*	Default constructor.
	*/
	PostProcessingBloomRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the post processing bloom render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the post processing bloom.
	*/
	void RenderInternal() NOEXCEPT;

};