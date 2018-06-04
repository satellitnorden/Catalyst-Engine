#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

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