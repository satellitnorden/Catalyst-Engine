#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class GrassDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(GrassDepthRenderPass);

	/*
	*	Default constructor.
	*/
	GrassDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the grass depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the grass.
	*/
	void RenderInternal() NOEXCEPT;

};