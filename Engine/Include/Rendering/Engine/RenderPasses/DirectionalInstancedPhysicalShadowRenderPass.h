#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalInstancedPhysicalShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalInstancedPhysicalShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalInstancedPhysicalShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional instanced physical shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional terrain shadow.
	*/
	void RenderInternal() NOEXCEPT;

};