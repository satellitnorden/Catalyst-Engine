#pragma once

//Core.
#include <Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class OceanRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(OceanRenderPass);

	/*
	*	Default constructor.
	*/
	OceanRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the ocean render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the ocean.
	*/
	void RenderInternal() NOEXCEPT;

};