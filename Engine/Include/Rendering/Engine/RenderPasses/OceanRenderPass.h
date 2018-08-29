#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

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