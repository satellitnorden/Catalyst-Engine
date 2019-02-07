#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class LowDetailTreeVegetationCrownColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LowDetailTreeVegetationCrownColorRenderPass);

	/*
	*	Default constructor.
	*/
	LowDetailTreeVegetationCrownColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the low detail tree vegetation crown color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the low detail tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};