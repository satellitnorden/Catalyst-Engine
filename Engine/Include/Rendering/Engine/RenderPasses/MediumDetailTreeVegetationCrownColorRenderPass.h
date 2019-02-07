#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailTreeVegetationCrownColorRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailTreeVegetationCrownColorRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailTreeVegetationCrownColorRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail tree vegetation crown color render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the color of the medium detail tree vegetation crown.
	*/
	void RenderInternal() NOEXCEPT;

};