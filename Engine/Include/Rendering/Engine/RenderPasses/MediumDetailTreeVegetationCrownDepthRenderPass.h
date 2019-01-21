#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailTreeVegetationCrownDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailTreeVegetationCrownDepthRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailTreeVegetationCrownDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail tree vegetation crown depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the medium detail tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};