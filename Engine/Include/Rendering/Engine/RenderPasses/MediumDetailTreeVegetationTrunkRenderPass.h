#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailTreeVegetationTrunkRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailTreeVegetationTrunkRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailTreeVegetationTrunkRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail tree vegetation trunk render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the medium detail tree vegetation trunks.
	*/
	void RenderInternal() NOEXCEPT;

};