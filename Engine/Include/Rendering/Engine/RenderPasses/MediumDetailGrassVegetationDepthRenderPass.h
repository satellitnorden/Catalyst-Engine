#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class MediumDetailGrassVegetationDepthRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MediumDetailGrassVegetationDepthRenderPass);

	/*
	*	Default constructor.
	*/
	MediumDetailGrassVegetationDepthRenderPass() NOEXCEPT;

private:

	/*
	*	Initializes the medium detail grass vegetation depth render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the depth of the medium detail grass vegetation.
	*/
	void RenderInternal() NOEXCEPT;

};