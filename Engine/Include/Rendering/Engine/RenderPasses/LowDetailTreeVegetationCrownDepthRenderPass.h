#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailTreeVegetationCrownDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailTreeVegetationCrownDepthRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailTreeVegetationCrownDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail tree vegetation crown depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the low detail tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};