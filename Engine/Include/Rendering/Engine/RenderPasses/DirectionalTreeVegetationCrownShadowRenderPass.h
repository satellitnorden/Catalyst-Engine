#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class DirectionalTreeVegetationCrownShadowRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DirectionalTreeVegetationCrownShadowRenderPass);

	/*
	*	Default constructor.
	*/
	DirectionalTreeVegetationCrownShadowRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the directional tree vegetation crown shadow render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the shadows of the directional tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};