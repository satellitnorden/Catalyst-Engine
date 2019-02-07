#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalSolidVegetationShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalSolidVegetationShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalSolidVegetationShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional solid vegetation shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the directional solid vegetation shadow.
	*/
	void RenderInternal() NOEXCEPT;

};