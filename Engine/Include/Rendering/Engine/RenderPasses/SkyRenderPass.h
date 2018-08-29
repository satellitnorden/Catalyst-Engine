#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class SkyRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(SkyRenderPass);

	/*
	*	Default constructor.
	*/
	SkyRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the sky render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the sky.
	*/
	void RenderInternal() NOEXCEPT;

};