#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTreeVegetationCrownDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTreeVegetationCrownDepthRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTreeVegetationCrownDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail tree vegetation crown depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the high detail tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};