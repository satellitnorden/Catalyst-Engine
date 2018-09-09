#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalDynamicPhysicalShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalDynamicPhysicalShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalDynamicPhysicalShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional dynamic physical shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional dynamic physical entity shadows.
	*/
	void RenderInternal() NOEXCEPT;

};