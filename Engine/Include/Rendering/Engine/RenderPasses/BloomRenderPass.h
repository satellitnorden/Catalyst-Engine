#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class BloomRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomRenderPass);

	/*
	*	Default constructor.
	*/
	BloomRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the bloom render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the bloom.
	*/
	void RenderInternal() NOEXCEPT;

};