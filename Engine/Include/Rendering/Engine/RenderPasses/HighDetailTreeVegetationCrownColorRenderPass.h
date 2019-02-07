#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class HighDetailTreeVegetationCrownColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(HighDetailTreeVegetationCrownColorRenderPass);

	/*
	*	Default constructor.
	*/
	HighDetailTreeVegetationCrownColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the high detail tree vegetation crown color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the high detail tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};