#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalStaticPhysicalShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalStaticPhysicalShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalStaticPhysicalShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional static physical shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional static physical entity shadows.
	*/
	void RenderInternal() NOEXCEPT;

};